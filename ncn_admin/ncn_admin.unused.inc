<?php
// Page Handler (admin/user/user/unactivated_member_list) - *Unused*
// See unactivated_member_list() - ncn_create_user.module
function ncn_admin_unactivated_member_list() {
GLOBAL $base_url;
    if (arg(4)== 'member_delete') {
        ncn_admin_unactivated_member_delete(arg(5));
    }
    drupal_add_js(drupal_get_path('module', 'ncn_admin').'/ncn_ admin.js');
    
    $header = array(
        array('data' => t('Member ID')),
        array('data' => t('Name')),
        array('data' => t('Status')), 
        array('data' => t('Company')),
        array('data' => t('Type')),
        array('data' => t('Contact Email')),
        array('data' => t('SignUp Date')),
        array('data' => t('Owner')),
        array('data' => t('Operation'))
    );
    
//  $query = "SELECT * FROM member_id_pool WHERE used=0 ORDER BY id DESC";
    $result = db_select('member_id_pool', 'n')
        ->fields('n')
        ->condition('used', 0,'=')
        ->orderBy('id', 'DESC')
        ->execute();
//  $result = pager_query($query, 50);
    //while ($row = db_fetch_array($result))
    foreach($result as $row)
    {
        $row = (array)$row;
        $member_type = "Gold";
        if ($row['member_type']==0) {
            $member_type = "Gold";
        }   else if ($row['member_type']==1) {
            $member_type = "Silver";
        } else if ($row['member_type']==2) {
            $member_type = "Gold Lite";
        } else if ($row['member_type']==3) {
            $member_type = "Coach on Call";
        } else if ($row['member_type']==4) {
            $member_type = "Gold Coach";
        } else if ($row['member_type']==5) {
            $member_type = "CSI Member";
        }
        
        if (ncn_admin_is_gold_member_demo($row['member_id'])) {
            $member_type .= " (Demo)";
        } else if (ncn_admin_is_member_first_free($row['member_id'])) {
            $member_type .= " (First Free)";
        }
        // is account free?
        $free_extra = '';
        if (is_member($row['member_id']))
        {   
            $result = db_query('SELECT customerProfileId FROM {member_cim} WHERE member_id=:s',array(':s'=>$row['member_id']))->fetchField();
            if (intval($result) == 0) {
                $free_extra = ' (FREE)';    
            }
        }
        $status = 'unactivated';
        if ($row['status'] == 0) {
            $status = 'paused';
        }
        
        $_owner = user_load($row['owner']);
        $rows[] = array('data' => array(
            // Cells
            $row['member_id'],
            $row['first_name'].' '.$row['last_name'] . $free_extra,
            $status, 
            $row['legalname'], 
            $member_type, 
            $row['contactemail'],
            date('d M Y', $row['create']),
            $_owner->profile_firstname.' '.$_owner->profile_lastname,
            "<a href='$base_url/admin/config/ncn_create_user/unactivated_member_list/edit/".$row['member_id']."'>Edit</a>&nbsp;&nbsp;".
//"<a href='$base_url/admin/user/user/unactivated_member_list/member_delete/".$row['member_id']."' onclick='return on_click_ncn_admin_una_member_delete();'>Delete</a>&nbsp;&nbsp;"
            "<a href='$base_url/admin/config/ncn_create_user/unactivated_member_list/member_delete/".$row['member_id']."' onclick='return on_click_ncn_admin_una_member_delete();'>Delete</a>&nbsp;&nbsp;"

        ),
        );
    }
    if (!$rows) {
        $rows[] = array(array('data' => t("There isn't unactivated member."), 'colspan' => 9));
    }
    $output = '';
    $output .= theme('table',array("header"=>$header,"rows"=>$rows, 'attributes'=>array('id' => 'una_member_list')));
    $output .= theme('pager');
    echo $output;
}

function ncn_admin_unactivated_member_delete($member_id) {
    $result = db_delete('member_id_pool')
        ->condition('member_id', $member_id)
        ->condition('used', 0,'=')
        ->execute();
    //    db_query("DELETE FROM {member_id_pool} WHERE member_id=':s' AND used=0",array(":s"=>$member_id));
    if ($result) {
        db_delete('member_cim')
            ->condition('member_id', $member_id)
            ->execute();
        db_delete('member_first_free')
            ->condition('member_id', $member_id)
            ->execute();
        db_delete('member_gold_demo')
            ->condition('member_id', $member_id)
            ->execute();

    //    db_query( "DELETE FROM {member_cim} WHERE member_id=':s'",array(":s"=>$member_id));
    //  db_query( "DELETE FROM {member_first_free} WHERE member_id=':s'",array(":s"=>$member_id ));
    //  db_query( "DELETE FROM {member_gold_demo} WHERE member_id=':s'",array(":s"=>$member_id));

        drupal_set_message( t('Member(#!member_id) deleted, successfully.', array('!member_id'=>$member_id)) );
    } else {
        drupal_set_message( t('Failed to delete member(#!member_id).', array('!member_id'=>$member_id)) );
    }

    drupal_goto('admin/config/ncn_create_user/unactivated_member_list');
}

// Done in ncn_create_user - function unactivated_member_list_edit()
// Unused this function.
// function ncn_admin_una_member_edit($member_id) {
// }

//------------------------------------------------------------------------------
// see user.admin.inc
function user_add_distributor()
{
    // user tried to add a distributor
    if (isset($_POST['tfunction']))
    {
        // error checking
        $error = false;
        
        foreach ($_POST as $key=>$val)
        {
            if (($_POST[$key] == '') && ($error == false))
            {
                drupal_set_message("All fields must be completed.", 'error');
                $error = true;
            }
        }
        
        // check valid username
        if ($error == false)
        {
        //  $query = "SELECT * FROM users WHERE `name`=\"".AddSlashes($_POST['name'])."\";";
            $result = db_query('SELECT * FROM {users} WHERE name=:a',
                array(':a'=>$_POST['name']));
            
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("That username is taken.", 'error');
                    $error = true;
                }           
            }
        }
        if ($error == false) {
            if (!valid_email_address($_POST['mail'])) {
                drupal_set_message('Email address is invalid, Please retry to input email address.', 'error');
                $error = true;
            }
        }
        
        // check passwords match        
        if ($error == false)
        {
            if ($_POST['pass'] != $_POST['pass2'])
            {
                drupal_set_message("Passwords do not match.", 'error');
                $error = true;
            }
        }
        if ($error == false)
        {
            if ($_POST['reportpass'] != $_POST['reportpass2'])
            {
                drupal_set_message("Report Passwords do not match.", 'error');
                $error = true;
            }
        }
        
        // check valid email
        /*if ($error == false)
        {
            $query = "SELECT * FROM users WHERE `mail`=\"".AddSlashes($_POST['mail'])."\";";
            $result = db_query($query);
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("Warining: That email address is already in use.", 'error');
                    $error = true;
                }   
            }           
        }*/
        
        // no errors, so try the card
        if ($error == false)
        {
            $error = !attempt_add_distributor_card();
        }
        
        // add good, so add the distributor
        if ($error == false)
        {
            $section1 = $_POST;
            
            unset($section1['pass2']);
            unset($section1['tfunction']);
            $section1['status'] = 1;
            $section1['roles'] = array(5 => 5, 2 => 1, 6 => 0, 7 => 0, 3 => 0, 4 => 0); 

            $section2 = array(
                'pass' => $_POST['pass'],
                'init' => $_POST['mail'],
                'roles' => array(5 => 5, 2 => 1)
            );
        
        
            $account = user_save('', array_merge($section1, $section2));
                          
            // Terminate if an error occured during user_save().
            if (!$account) {
                drupal_set_message(t("Error saving user account."), 'error');
                $form_state['redirect'] = '';
                $error = true;
            }
            
            // send welcome email (account manager) HERE
            $params = array(
                '!username' => $_POST['name'],
                '!password' => $_POST['pass'],
                '!firstname' => $_POST['profile_firstname'],
                '!lastname' => $_POST['profile_lastname'],
            );
            add_report_password($_POST['name'], $_POST['reportpass']);
            
            drupal_mail('ncn_admin', 'distributor', $_POST['mail'], language_default(), $params);
            
        }
        
        // everything worked - show success
        if ($error == false)
        {
            // save the ownersship relationship
        //  $query = "INSERT INTO users_parent (uid, parent) VALUES(".$account->uid.", ".$_POST['account_manager'].");";
            $result = db_query('INSERT INTO {users_parent} (uid, parent) VALUES(:a,:b)',
                array(':a'=>$account->uid,':b'=>$_POST['account_manager']));
            
            $_POST = array();
            drupal_set_message("Distributor '".$_POST['name']."' added successfully", 'status');
        }
        
         
    }

    // ---- draw the form ----

    ?>
    <form method="POST" autocomplete="off">
    <input type="hidden" name="tfunction" value="add_distributor">

    <fieldset>
    <legend>Create new distributor</legend>
    
        
        <table style="width: 600px; border:0;">
            <tr>
                <td>Username</td>
                <td><input type="text" name="name" value="<?= $_POST['name']; ?>"></td>
            </tr>
            <tr>
                <td>Assign Account Manager</td>
                <td><?php account_manager_dropdown(); ?></td>
            </tr>
            <tr>
                <td>E-mail</td>
                <td><input type="text" name="mail" value="<?= $_POST['mail']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="password" name="pass" value="<?= $_POST['pass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Confirm Password</td>
                <td><input type="password" name="pass2" value="<?= $_POST['pass2']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>First Name</td>
                <td><input type="text" name="profile_firstname" value="<?= $_POST['profile_firstname']; ?>"></td>
            </tr>
            <tr>
                <td>Last Name</td>
                <td><input type="text" name="profile_lastname" value="<?= $_POST['profile_lastname']; ?>"></td>
            </tr>
            <tr>
                <td>Company's Name</td>
                <td><input type="text" name="profile_legalname" value="<?= $_POST['profile_legalname']; ?>"></td>
            </tr>
            <tr>
                <td>Country</td>
                <td><?php draw_countryform_part('profile_country', false, false); ?></td>
            </tr>
            <tr>
                <td>Address</td>
                <td><input type="text" name="profile_address" value="<?= $_POST['profile_address']; ?>"></td>
            </tr>
            <tr>
                <td>City</td>
                <td><input type="text" name="profile_city" value="<?= $_POST['profile_city']; ?>"></td>
            </tr>
            <tr>
                <td>Province/State</td>
                <td><?php draw_stateform_part('profile_state', false, false); ?></td>
            </tr>
            <tr>
                <td>Zip</td>
                <td><input type="text" name="profile_zip" value="<?= $_POST['profile_zip']; ?>"></td>
            </tr>

            <tr>
                <td>Credit Card Number</td>
                <td><input type="text" name="profile_credit_card_number" value="<?= $_POST['profile_credit_card_number']; ?>"></td>
            </tr>
            <tr>
                <td>Expiration Date (mm/yy)</td>
                <td><input type="text" name="profile_credit_card_expiration" value="<?= $_POST['profile_credit_card_expiration']; ?>"></td>
            </tr>
            <tr>
                <td>Security Code</td>
                <td><input type="text" name="profile_credit_card_security" value="<?= $_POST['profile_credit_card_security']; ?>"></td>
            </tr>
            <tr>
                <td>Name as it appears on card</td>
                <td><input type="text" name="profile_credit_card_name" value="<?= $_POST['profile_credit_card_name']; ?>"></td>
            </tr>
            <tr>
                <td>Report Password</td>
                <td><input type="password" name="reportpass" value="<?= $_POST['reportpass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Confirm Password</td>
                <td><input type="password" name="reportpass2" value="<?= $_POST['reportpass2']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td colspan="2"><input type="submit" value="Add Distributor"></td>
            </tr>
        </table>
    </fieldset>
    
    </form> 
    <?php

}
//------------------------------------------------------------------------------
//----------------lead_generation_user--------------------------
function lead_generation_user()
{

    // user tried to add a lead generation user
    if (isset($_POST['tfunction']))
    {
        // error checking
        $error = false;
        $flage= true;
        
        foreach ($_POST as $key=>$val)
        {
            if (($_POST[$key] == '') && ($error == false))
            {
                drupal_set_message("All fields must be completed.", 'error');
                $error = true;
                $flage= false;
            }
        }
        
/*      if ($error == false) {
        if ($_POST['parent_mid'] == '') {
            drupal_set_message("Please select member.", 'error');
            $error = true;
        }
    }*/
        
        // check valid username
        if ($error == false)
        {
    //      $query = "SELECT * FROM users WHERE `name`=\"".AddSlashes($_POST['name'])."\";";
            $result = db_query('SELECT * FROM {users} WHERE name=:a',array(':a'=>$_POST['name']));
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("That username is taken.", 'error');
                    $error = true;
                }   
            }           
        }
        if ($error == false) {
            if (!valid_email_address($_POST['mail'])) {
                drupal_set_message('Email address is invalid, Please retry to input email address.', 'error');
                $error = true;
            }
        }
        
        // check passwords match        
        if ($error == false)
        {
            if ($_POST['pass'] != $_POST['pass2'])
            {
                drupal_set_message("Passwords do not match.", 'error');
                $error = true;
            }
        }
        if ($error == false)
        {
            if ($_POST['reportpass'] != $_POST['reportpass2'])
            {
                drupal_set_message("Report Passwords do not match.", 'error');
                $error = true;
            }
        } 
        
        // check valid email
        if ($error == false)
        {
    //      $query = "SELECT * FROM users WHERE `mail`=\"".AddSlashes($_POST['mail'])."\";";
            $result = db_query('SELECT * FROM {users} WHERE mail=:a',array(':a'=>$_POST['mail']));
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("Warining: That email address is already in use.", 'error');
                    $error = true;
                }   
            }           
        }
        
    if (!$error) {
            
            require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
            
            $name="'".$_POST['name']."'";
            $pass="'". user_hash_password(trim($_POST['pass']))."'";
            $mail="'".$_POST['mail']."'";
            $firstname = "'".$_POST['firstname']."'";
            $lastname =  "'".$_POST['lastname']."'";
            $company_name = "'".$_POST['company_name']."'";
            $dist_id = $_POST['parent_mid'];
            
            $date = strtotime(date('Y-m-d H:i:s'));
            $user_id = db_next_id("{users}");
            $success = db_query("INSERT INTO {users} (`uid`,`name`,`mail`,`pass`,`created`,`status`,`init`) VALUES ($user_id,$name,$mail,$pass,$date,'1',$mail)");
           
            if ($success) {
               // $last_id = db_query("SELECT lead_id FROM users_lead ORDER BY lead_id DESC LIMIT 1");
               $last_id = db_query("SELECT Max( Substr( lead_id, 4 ) ) as max_id FROM users_lead ");
               $last_id = (array)$last_id;
                //print_r($last_id);exit;
            if($last_id['max_id'] != '')
            {
                //$temp_id = substr($last_id,3,strlen($last_id));
                $last_id_str=$last_id['max_id']+1;
                 $lead_id = "'LVC".$last_id_str."'";
            }
            else
            {
                 $lead_id = "'LVC101'";
            }
            
           $success1 = db_query("INSERT INTO {users_lead} (`lead_id`,`user_id`,`distributor_id`,`company_name`,`firstname`,`lastname`) VALUES ($lead_id,$user_id,'NULL',$company_name,$firstname,$lastname)");
           $success2 = db_query("INSERT INTO {users_roles} (`uid`,`rid`) VALUES ($user_id,'23')");
            }
            else
            {
                    drupal_set_message("lead generation user not added");
            }
           
           if ($success1) {
                $_POST = array();
                drupal_set_message("lead generation user added successfully");
            }
            else
            {
                    drupal_set_message("lead generation user not added");
            }
            
        }
}
    ?>
    <form method="POST" autocomplete="off">
    <input type="hidden" name="tfunction" value="add_lead_generation_user">

    <fieldset>
    <legend>Create new lead generation user</legend>
    
        
        <table style="width: 600px; border:0;">
            <tr>
                <td>Username</td>
                <td><input type="text" name="name" value="<?= $_POST['name']; ?>"></td>
            </tr>
            <tr>
                <td>E-mail</td>
                <td><input type="text" name="mail" value="<?= $_POST['mail']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="password" name="pass" value="<?= $_POST['pass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Confirm Password</td>
                <td><input type="password" name="pass2" value="<?= $_POST['pass2']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>First Name</td>
                <td><input type="text" name="firstname" value="<?= $_POST['firstname']; ?>"></td>
            </tr>
            <tr>
                <td>Last Name</td>
                <td><input type="text" name="lastname" value="<?= $_POST['lastname']; ?>"></td>
            </tr>
            <tr>
                <td>Company's Name</td>
                <td><input type="text" name="company_name" value="<?= $_POST['company_name']; ?>"></td>
            </tr>
            <?php /*<tr>
                <td>Assign Member</td>
                <td><?php print ncn_subuser_member_dropdown('parent_mid', $_POST['parent_mid']); ?></td>
            </tr> */ ?>
            <tr>
                <td colspan="2"><input type="submit" value="Add Lead Generator User"></td>
            </tr>
        </table>
    </fieldset>
    
    </form> 
    <?php
}
//------------------------------------------------------------------------------
// see user.admin.inc
function user_add_associate()
{
// user tried to add a associate
    if (isset($_POST['tfunction']))
    {
        // error checking
        $error = false;
        
        foreach ($_POST as $key=>$val)
        {
            if (($_POST[$key] == '') && ($error == false))
            {
                drupal_set_message("All fields must be completed.", 'error');
                $error = true;
            }
        }
        
        // check valid username
        if ($error == false)
        {
            $query = "SELECT * FROM users WHERE `name`=\"".AddSlashes($_POST['name'])."\";";
            $result = db_query($query);
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("That username is taken.", 'error');
                    $error = true;
                }   
            }           
        }
        
        if ($error == false) {
            if (!valid_email_address($_POST['mail'])) {
                drupal_set_message('Email address is invalid, Please retry to input email address.', 'error');
                $error = true;
            }
        }

        // check passwords match        
        if ($error == false)
        {
            if ($_POST['pass'] != $_POST['pass2'])
            {
                drupal_set_message("Passwords do not match.", 'error');
                $error = true;
            }
        }
        
        // check valid email
        if ($error == false)
        {
            $query = 'SELECT * FROM users WHERE mail = :mail';
            $result = db_query($query,array(':mail'=>addslashes($_POST['mail'])));
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("Warining: That email address is already in use.", 'error');
                    // $error = true;
                }   
            }           
        }
        
        // no errors, so try the card // associate doesn't need credit card
        /*if ($error == false)
        {
            $error = !attempt_add_distributor_card();
        }*/
        
        // add good, so add the associate
        if ($error == false)
        {
            $section1 = $_POST;
            
            unset($section1['pass2']);
            unset($section1['tfunction']);
            $section1['status'] = 1;
            $section1['roles'] = array(7 => 7, 2 => 1, 5 => 0, 6 => 0, 3 => 0, 4 => 0); 

            $section2 = array(
                'pass' => $_POST['pass'],
                'init' => $_POST['mail'],
                'roles' => array(7 => 7, 2 => 1)
            );
        
        
            $account = user_save('', array_merge($section1, $section2));
                          
            // Terminate if an error occured during user_save().
            if (!$account) {
                drupal_set_message(t("Error saving user account."), 'error');
                $form_state['redirect'] = '';
                $error = true;
            }
            
            // send welcome email (account manager) HERE
            $params = array(
                '!username' => $_POST['name'],
                '!password' => $_POST['pass'],
                '!firstname' => $_POST['profile_firstname'],
                '!lastname' => $_POST['profile_lastname'],
            );
                        
            drupal_mail('ncn_admin', 'associate', $_POST['mail'], language_default(), $params);
            
        }
        
        // everything worked - show success
        if ($error == false)
        {
            // save the ownersship relationship
            $query = "INSERT INTO users_parent (uid, parent) VALUES(".$account->uid.", ".$_POST['account_manager'].");";
            $result = db_query($query);
        
            $_POST = array();
            drupal_set_message("Associate '".$_POST['name']."' added successfully", 'status');
        }
        
         
    }

    // ---- draw the form ----

    ?>
    <form method="POST">
    <input type="hidden" name="tfunction" value="add_associate">

    <fieldset>
    <legend>Create new Assoicate</legend>
    
        
        <table style="width:400px;border:0;">
            <tr>
                <td>Username</td>
                <td><input type="text" name="name" value="<?= $_POST['name']; ?>"></td>
            </tr>
            <tr>
                <td>Assign Account Manager</td>
                <td><?php account_manager_dropdown(); ?></td>
            </tr>
            <tr>
                <td>E-mail</td>
                <td><input type="text" name="mail" value="<?= $_POST['mail']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="password" name="pass" value="<?= $_POST['pass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Confirm Password</td>
                <td><input type="password" name="pass2" value="<?= $_POST['pass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>First Name</td>
                <td><input type="text" name="profile_firstname" value="<?= $_POST['profile_firstname']; ?>"></td>
            </tr>
            <tr>
                <td>Last Name</td>
                <td><input type="text" name="profile_lastname" value="<?= $_POST['profile_lastname']; ?>"></td>
            </tr>
            <tr>
                <td>Company's Name</td>
                <td><input type="text" name="profile_legalname" value="<?= $_POST['profile_legalname']; ?>"></td>
            </tr>
            <tr>
                <td>Country</td>
                <td><?php draw_countryform_part('profile_country', false, false); ?></td>
            </tr>
            <tr>
                <td>Address</td>
                <td><input type="text" name="profile_address" value="<?= $_POST['profile_address']; ?>"></td>
            </tr>
            <tr>
                <td>City</td>
                <td><input type="text" name="profile_city" value="<?= $_POST['profile_city']; ?>"></td>
            </tr>
            <tr>
                <td>Province/State</td>
                <td><?php draw_stateform_part('profile_state', false, false); ?></td>
            </tr>
            <tr>
                <td>Zip</td>
                <td><input type="text" name="profile_zip" value="<?= $_POST['profile_zip']; ?>"></td>
            </tr>

            <!-- <tr>
                <td>Credit Card Number</td>
                <td><input type="text" name="profile_credit_card_number" value="<?= $_POST['profile_credit_card_number']; ?>"></td>
            </tr>
            <tr>
                <td>Expiration Date (mm/yy)</td>
                <td><input type="text" name="profile_credit_card_expiration" value="<?= $_POST['profile_credit_card_expiration']; ?>"></td>
            </tr>
            <tr>
                <td>Security Code</td>
                <td><input type="text" name="profile_credit_card_security" value="<?= $_POST['profile_credit_card_security']; ?>"></td>
            </tr>
            <tr>
                <td>Name as it appears on card</td>
                <td><input type="text" name="profile_credit_card_name" value="<?= $_POST['profile_credit_card_name']; ?>"></td>
            </tr> -->
            
            <tr>
                <td colspan="2"><input type="submit" value="Add Associate"></td>
            </tr>
        </table>
    </fieldset>
    
    </form> 
    <?php
}

//------------------------------------------------------------------------------
// see user.admin.inc
function user_add_sub_user()
{
// user tried to add a associate
    if (isset($_POST['tfunction']))
    {
        $parent_mid = $_POST['parent_mid'];
        if ( module_exists('ncn_subuser') ) {
            if (ncn_subuser_create_subuser($parent_mid, $_POST)) {
                $_POST = array();
            }
        }
    }

    // ---- draw the form ----

    ?>
    <form method="POST">
    <input type="hidden" name="tfunction" value="create-subuser">

    <fieldset>
    <legend>Create new Sub User</legend>
    
        
        <table style="width:400px;border:0;">
            <tr>
                <td>Username</td>
                <td><input type="text" name="name" value="<?= $_POST['name']; ?>"></td>
            </tr>
            <tr>
                <td>Assign Member</td>
                <td><?php print ncn_subuser_member_dropdown('parent_mid', $_POST['parent_mid']); ?></td>
            </tr>
            <tr>
                <td>E-mail</td>
                <td><input type="text" name="mail" value="<?= $_POST['mail']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="password" name="pass" value="<?= $_POST['pass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>Confirm Password</td>
                <td><input type="password" name="pass2" value="<?= $_POST['pass']; ?>" AUTOCOMPLETE="off"></td>
            </tr>
            <tr>
                <td>First Name</td>
                <td><input type="text" name="profile_firstname" value="<?= $_POST['profile_firstname']; ?>"></td>
            </tr>
            <tr>
                <td>Last Name</td>
                <td><input type="text" name="profile_lastname" value="<?= $_POST['profile_lastname']; ?>"></td>
            </tr>
            <tr>
                <td>Company's Name</td>
                <td><input type="text" name="profile_legalname" value="<?= $_POST['profile_legalname']; ?>"></td>
            </tr>
            <tr>
                <td>Country</td>
                <td><?php draw_countryform_part('profile_country', false, false); ?></td>
            </tr>
            <tr>
                <td>Address</td>
                <td><input type="text" name="profile_address" value="<?= $_POST['profile_address']; ?>"></td>
            </tr>
            <tr>
                <td>City</td>
                <td><input type="text" name="profile_city" value="<?= $_POST['profile_city']; ?>"></td>
            </tr>
            <tr>
                <td>Province/State</td>
                <td><?php draw_stateform_part('profile_state', false, false); ?></td>
            </tr>
            <tr>
                <td>Zip</td>
                <td><input type="text" name="profile_zip" value="<?= $_POST['profile_zip']; ?>"></td>
            </tr>
            
            <tr>
                <td colspan="2"><input type="submit" value="Add Sub User"></td>
            </tr>
        </table>
    </fieldset>
    
    </form> 
    <?php
}

//------------------------------------------------------------------------------
// see user.admin.inc
function user_add_account_manager()
{
    // user tried to add a distributor
    if (isset($_POST['tfunction']))
    {
        // error checking
        $error = false;
        
        foreach ($_POST as $key=>$val)
        {
            if (($_POST[$key] == '') && ($error == false))
            {
                drupal_set_message("All fields must be completed.", 'error');
                $error = true;
            }
        }

        // check valid username
        if ($error == false)
        {
        //  $query = "SELECT * FROM users WHERE `name`=\"".AddSlashes($_POST['name'])."\";";
            $result = db_query('SELECT * FROM {users} WHERE name=:a',array(':a'=>$_POST['name']));
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("That username is taken.", 'error');
                    $error = true;
                }   
            }           
        }
        
        if ($error == false) {
            if (!valid_email_address($_POST['mail'])) {
                drupal_set_message('Email address is invalid, Please retry to input email address.', 'error');
                $error = true;
            }
        }
        
        // check passwords match        
        if ($error == false)
        {
            if ($_POST['pass'] != $_POST['pass2'])
            {
                drupal_set_message("Passwords do not match.", 'error');
                $error = true;
            }
        }
        
        
        // check valid email
        if ($error == false)
        {
        //  $query = "SELECT * FROM users WHERE `mail`=\"".AddSlashes($_POST['mail'])."\";";
            $result = db_query('SELECT * FROM {users} WHERE mail=:a',array(':a'=>$_POST['mail']));
            foreach($result as $row)
            {
                $row = (array)$row;         
                if(count($row) > 0)
                {
                    drupal_set_message("That email address is already in use.", 'error');
                    $error = true;
                }
            }           
        }
        
        $role_array = user_roles();
        // add good, so add the account manager
        if ($error == false)
        {
            $section1 = $_POST;
            
            unset($section1['pass2']);
            unset($section1['tfunction']);
            unset($section1['distributor']);
            $section1['status'] = 1;
            //$section1['roles'] = array(5 => 0, 7 => 0, 2 => 1, 6 => 6, 3 => 0, 4 => 0);   

            $section2 = array(
                'pass' => $_POST['pass'],
                'init' => $_POST['mail'],
                'roles' => array($_POST['user_role'] => $_POST['user_role'], 2 => 1)
            );
            
            $account = user_save('', array_merge($section1, $section2));
                          
            // Terminate if an error occured during user_save().
            if (!$account) {
                drupal_set_message(t("Error saving user account."), 'error');
                $form_state['redirect'] = '';
                $error = true;
            }
            
            // send welcome email (account manager) HERE
            $params = array(
                '!username' => $_POST['name'],
                '!password' => $_POST['pass'],
                '!firstname' => $_POST['profile_firstname'],
                '!lastname' => $_POST['profile_lastname'],
                '!rolename' => ucwords($role_array[$_POST['user_role']]),
            );
                        
         drupal_mail('ncn_admin', 'accountmanager', $_POST['mail'], language_default(), $params);
            
        }
        
        // everything worked - show success
        if ($error == false)
        {
            drupal_set_message(ucwords($role_array[$_POST['user_role']]). " " . $_POST['name']." added successfully", 'status');
        }
        
         
    }

    // ---- draw the form ----

    ?>
    <form method="POST" autocomplete="off">
    <input type="hidden" name="tfunction" value="add_distributor">

    <fieldset>
    <legend>Create New User</legend>
    
        
        <table style="width:400px;border:0;">
            <tr>
                <td>Username</td>
                <td><input type="text" name="name" value="<?= $_POST['name']; ?>"></td>
            </tr>
            <tr>
                <td>E-mail</td>
                <td><input type="text" name="mail" value="<?= $_POST['mail']; ?>" autocomplete="off"></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="password" name="pass" value="<?= $_POST['pass']; ?>" autocomplete="off"></td>
            </tr>
            <tr>
                <td>Confirm Password</td>
                <td><input type="password" name="pass2" value="<?= $_POST['pass']; ?>" autocomplete="off"></td>
            </tr>
            <tr>
                <td>Role</td>
                <td><?php echo ncn_admin_render_select_role('user_role', '', ''); ?></td>
            </tr>
            <tr>
                <td>First Name</td>
                <td><input type="text" name="profile_firstname" value="<?= $_POST['profile_firstname']; ?>"></td>
            </tr>
            <tr>
                <td>Last Name</td>
                <td><input type="text" name="profile_lastname" value="<?= $_POST['profile_lastname']; ?>"></td>
            </tr>
            <tr>
                <td>Company's Name</td>
                <td><input type="text" name="profile_legalname" value="<?= $_POST['profile_legalname']; ?>"></td>
            </tr>
            <tr>
                <td>Country</td>
                <td><?php draw_countryform_part('profile_country', false, false); ?></td>
            </tr>
            <tr>
                <td>Address</td>
                <td><input type="text" name="profile_address" value="<?= $_POST['profile_address']; ?>"></td>
            </tr>
            <tr>
                <td>City</td>
                <td><input type="text" name="profile_city" value="<?= $_POST['profile_city']; ?>"></td>
            </tr>
            <tr>
                <td>Province/State</td>
                <td><?php draw_stateform_part('profile_state', false, false); ?></td>
            </tr>
            <tr>
                <td>Zip</td>
                <td><input type="text" name="profile_zip" value="<?= $_POST['profile_zip']; ?>"></td>
            </tr>

            <tr>
                <td colspan="2"><input type="submit" value="Add"></td>
            </tr>
        </table>
    </fieldset>
    
    </form> 
    <?php

}
