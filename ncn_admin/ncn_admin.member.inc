<?php
/**
 * URL: admin/config/ncn_admin/member_ids
 * Quick Add Member
 */
function ncn_admin_page_member_ids()
{
    GLOBAL $user;
    
    drupal_add_js(drupal_get_path('module', 'ncn_admin') . '/ncn_admin.js');
    
    // check for access denied
    if (isset($GLOBALS['user']->roles[6]))
    {
        //return("Access denied.");
    }

    $tfunction = isset($_POST['tfunction']) ? $_POST['tfunction'] : '';

    switch($tfunction)
    {
        case "download_pool":
            ncn_admin_member_download_pool_action();
            break;
        case "paste_ids":
            ncn_admin_quick_add_member_action();
            break;
        case "upload_ids": // - BREAK -
            ncn_admin_member_upload_ids_action();
            break;
    }


    ob_start();
    // render the page
    ?>
    <fieldset>
    <legend>Create new member</legend>
        <p>Input fields for a new member and click "submit" to add to the pool.</p> 
        <form method="POST" id="quick_add_member_form">
        <input type="hidden" name="tfunction" value="paste_ids" />
        
        <table style="width:400px;border:0;">
            <tr>
                <td>Member ID#</td>
                <td><input type="text" name="member_id" value="<?= generate_member_id() ?>" readonly></td>
            </tr>
            <tr>
                <td>First Name</td>
                <td><input type="text" name="first_name" value="<?= isset($_POST['first_name'])?$_POST['first_name']:''; ?>"></td>
            </tr>
            <tr>
                <td>Last Name</td>
                <td><input type="text" name="last_name" value="<?= isset($_POST['last_name'])?$_POST['last_name']:''; ?>"></td>
            </tr>
            <tr>
                <td>Company's Legal Name</td>
                <td><input type="text" name="legalname" value="<?= isset($_POST['legalname'])?$_POST['legalname']:''; ?>"></td>
            </tr>
            <tr>
                <td>Country</td>
                <td><?php draw_countryform_part('country', false, false); ?></td>
            </tr>
            <tr>
                <td>Address</td>
                <td><input type="text" name="address" value="<?= isset($_POST['address'])?$_POST['address']:''; ?>"></td>
            </tr>
            <tr>
                <td>City</td>
                <td><input type="text" name="city" value="<?= isset($_POST['city'])?$_POST['city']:''; ?>"></td>
            </tr>
            <tr>
                <td>Province/State</td>
                <td><?php draw_stateform_part('state', false, false); ?></td>
            </tr>
            <tr>
                <td>Zip</td>
                <td><input type="text" name="zip" value="<?= isset($_POST['zip'])?$_POST['zip']:''; ?>"></td>
            </tr>
            <tr>
                <td>Office Phone</td>
                <td><input type="text" name="officephone" value="<?= isset($_POST['officephone'])?$_POST['officephone']:''; ?>"></td>
            </tr>
            <tr>
                <td>Mobile Phone</td>
                <td><input type="text" name="mobilephone" value="<?= isset($_POST['mobilephone'])?$_POST['mobilephone']:''; ?>"></td>
            </tr>
            <tr>
                <td>Contact Email</td>
                <td><input type="text" name="contactemail" value="<?= isset($_POST['contactemail'])?$_POST['contactemail']:''; ?>"></td>
            </tr>
            <tr>
                <td>Sales Rep</td>
                <td><input type="text" name="sales_rep" value="<?= isset($_POST['sales_rep'])?$_POST['sales_rep']:''; ?>"></td>
            </tr>
            
            <!-- <tr class="leaduser-list">
                <td>Lead User</td>
                <td><?php /*  leaduser_dropdown(); */ ?></td>
            </tr> -->

            <tr class="distributor-list">
                <td>Type</td>
                <td>
                    <select id="dist_asso" name="dist_asso">
                        <option value="distributor">Distributor</option>
                        <option value="associate">Associate</option>
                    </select>
                </td>
            </tr>
            <tr class="distributor-list da-dropdown">
                <td>Distributor</td>
                <td><?php distributor_dropdown(); ?></td>
            </tr>
            <tr class="associate-list da-dropdown">
                <td>Associate</td>
                <td><?php associate_dropdown(); ?></td>
            </tr>
            <tr class="">
                <td>Account Manager</td>
                <td>
                    <?php 
                        if (isset($_POST['am_uid'])) {
                            echo draw_select_account_manager_list('am_uid',  $_POST['am_uid']); 
                        } else {
                            echo draw_select_account_manager_list('am_uid',  ncn_admin_get_auto_assigned_am());
                        }
                    ?>
                </td>
            </tr>
            
            <?php
            /*if ((is_distributor($GLOBALS['user']) == false) && (is_account_manager($GLOBALS['user']) == false) )
            {
                ?>
                <tr>
                    <td>Distributor</td>
                    <td><?php distributor_dropdown(); ?></td>
                </tr>
                <?php
            }
            else
            {
                ?>
                <tr>
                    <td>Distributor</td>
                    <td><?php distributor_dropdown(); ?></td>
                </tr>
                <?php
            }*/
            
            ?>
            <!-- <tr>
                <td colspan="2">
                <input type="checkbox" name="overwrite_existing"> Overwrite existing unactivated member
                </td>
            </tr> --> 
            <tr>
                <td>Member Type</td>
                <td>
                <?php
                    $member_type = 0;   
                    if (isset($_POST['member_type'])) {
                        $member_type = $_POST['member_type'];
                    }
                    ncn_admin_render_member_type($member_type);
                ?>
                </td>
            </tr> 
            <tr>
                <td>SignUp Fee Type</td>
                <td>
                <?php
                    $product_nid = SKU_RETAIL_KIT;
                    if (isset($_POST['retail_kit'])) {
                        $product_nid = $_POST['retail_kit'];
                        }
                    echo ncn_admin_draw_signup_fee_select('retail_kit', $product_nid); 
                ?>
                </td>
            </tr> 


        </table>

        <p>&nbsp;</p>
        <!-- <p>Credit/Debit Card Details (optional)</p> -->
        <table style="width:400px;border:0;">
            <tr>
                <td>Card Number</td>
                <td><input type="text" name="card_number" value="<?= isset($_POST['card_number'])?$_POST['card_number']:''; ?>">
            <tr>
            <tr>
                <td>Expiration Date (mm/yy)</td>
                <td><input type="text" name="expiration_date" value="<?= isset($_POST['expiration_date'])?$_POST['expiration_date']:''; ?>">
            <tr>
            <tr>
                <td>Security Code</td>
                <td><input type="text" name="security_code" value="<?= isset($_POST['security_code'])?$_POST['security_code']:''; ?>">
            <tr>
            
        </table>
        
    
        <p><input id="ncn_admin_quick_add_member_btn" type="submit" value="Submit" onclick="return ncn_change_submit_button_disabled('ncn_admin_quick_add_member_btn', 'Submitting...', 'quick_add_member_form')"></p>
        </form> 
    </fieldset>
    
    <?php
    
    if ((is_distributor($GLOBALS['user']) == false) && (is_associate($GLOBALS['user']) == false) && (is_account_manager($GLOBALS['user']) == false) )
    {
    ?>
    
    <!--
    <fieldset>
    <legend>Upload member ID's</legend>
    
        <p>Click "Browse/Choose File" below to attach a CSV file containing member ID's (one per line) and click "submit" to add them to the pool.</p>
        <p>Format must be: member id, first name, last name</p>
        <form method="POST" enctype="multipart/form-data" />
        <input type="hidden" name="tfunction" value="upload_ids" />
        
        <input type="file" name="ids" /><br />
        Distributor: <?php distributor_dropdown(); ?>
    
        <p><input type="submit" value="Submit"></p>
        </form> 
    
    </fieldset>
    -->
    
    
    <!-- <p><b>Download the existing pool of member ID's in CSV format (copmpatible with most spreadsheets and databases):</b></p>
    <form method="POST">
    <input type="hidden" name="tfunction" value="download_pool" />
    <p><input type="submit" value="Download Member ID List"></p>
    </form> -->
    <?php
    }
    
    // end the buffer
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
}

function ncn_admin_quick_add_member_action() {
    // sanitize input
    $member_id = $new_string = preg_replace("/[^a-zA-Z0-9\s]/", "", $_POST['member_id']);
    $first_name = $new_string = preg_replace("/[^a-zA-Z0-9\s\'\.]/", "", $_POST['first_name']);
    $last_name = $new_string = preg_replace("/[^a-zA-Z0-9\s\'\.]/", "", $_POST['last_name']);
    //$legalname = $new_string = preg_replace("/[^a-zA-Z0-9\s\'\.\&]/", "", $_POST['legalname']);
    $legalname = $new_string = $_POST['legalname'];
    
    $country = $new_string = preg_replace("/[^a-zA-Z0-9\s]/", "", $_POST['country']);
    $address = $new_string = preg_replace("/[^a-zA-Z0-9\s\'\.]/", "", $_POST['address']);
    $city = $new_string = preg_replace("/[^a-zA-Z0-9\s\'\.]/", "", $_POST['city']);
    $state = $new_string = preg_replace("/[^a-zA-Z0-9\s]/", "", $_POST['state']);
    $zip = $new_string = preg_replace("/[^a-zA-Z0-9\s]/", "", $_POST['zip']);
    $officephone = $new_string = preg_replace("/[^a-zA-Z0-9\s]/", "", $_POST['officephone']);
    $mobilephone = $new_string = preg_replace("/[^a-zA-Z0-9\s]/", "", $_POST['mobilephone']);
    $contactemail = $new_string = preg_replace("/[^a-zA-Z0-9\/:@\.\+-s]/", "", $_POST['contactemail']);
    $sales_rep = $new_string = preg_replace("/[^a-zA-Z0-9\s\'\.]/", "", $_POST['sales_rep']);
    
    $card_number = trim($_POST['card_number']); //if ($card_number == "") {$card_number = '5424000000000015'; }
    $expiration_date = trim($_POST['expiration_date']);
    $security_code = trim($_POST['security_code']);
    $am_uid = trim($_POST['am_uid']);
    //* validation */
    if ( (trim($member_id)=="") || (trim($first_name)=="") || (trim($last_name)=="") || (trim($legalname)=="") ||
         (trim($country)=="") || (trim($address)=="") || (trim($city)=="") || (trim($state)=="") ||
         (trim($zip)=="") || (trim($officephone)=="") || (trim($mobilephone)=="") || (trim($contactemail)=="") || (trim($sales_rep)=="") ||
         (trim($card_number)=="") || (trim($expiration_date)=="") || (trim($security_code)=="") || (trim($am_uid)=="") ){
         drupal_set_message("You need to input all fields.", "error");
         return;
    }
    
    if (!valid_email_address($contactemail)) {
        drupal_set_message('Contact email address is invalid. Please retry to input email address.', 'error');
        return;
    }
    
    // Check Duplication of Email
    if (check_existing_email($contactemail)) {
        drupal_set_message('Contact email address exists. Please use another email address.', 'error');
        return;
    }
    
    // get distributor or associate name
    $assign_whose = $_POST['dist_asso'];
    if ( $assign_whose == 'distributor' ) {
        $dist = user_load($_POST['distributor']);
    } else {
        $dist = user_load($_POST['associate']); // * add this code for associate role *
    }
    $distributor_name = $dist->profile_firstname.' '.$dist->profile_lastname;

    // check first existing member id
    // $query = "SELECT * FROM member_id_pool WHERE member_id='".addslashes($member_id)."' AND used=1;";
    $found_used = check_existing_memberid($member_id, 1);
    
    // $query = "SELECT * FROM member_id_pool WHERE member_id='".addslashes($member_id)."' AND used=0;";
    $found_unused = check_existing_memberid($member_id, 0);
    
    // allow add
    // if ($found_used == 0)
    // {
    //     if ( ($found_unused> 0) && (!isset($_POST['overwrite_existing'])) ) {   }
    //     else{   $allow_add = true;  }
    // }
    $allow_add = false;
    if ($found_used == 0 && $found_unused == 0) {
        $allow_add = true;
    }

    // add the member id
    if ($allow_add == true)
    {
        $member_type = 0; 
        if (isset($_POST['member_type'])) {
            $member_type = $_POST['member_type'];
        }
        // get singup fee type
        $product_nid = SKU_RETAIL_KIT;
        if (isset($_POST['retail_kit'])) {
            $product_nid = $_POST['retail_kit'];
        }
        $signup_fee_type = get_signup_fee_type($product_nid);
            
        // check credit card
        $add_cim_profile = false;
        $error = false;
        if (($card_number != '') && ($card_number != '5424000000000015'))
        {
            $cim_vars = array(
                'expiration_date'           => $expiration_date, 
                'merchant_customer_id'      => $member_id.date("U"), 
                'first_name'                => $first_name, 
                'last_name'                 => $last_name, 
                'contactemail'              => $contactemail, 
                'address'                   => $address, 
                'city'                      => $city, 
                'state'                     => $state, 
                'zip'                       => $zip, 
                'card_number'               => $card_number, 
                'security_code'             => $security_code, 
            );
            $cim = getNcnCimCustomerProfileInfo($cim_vars);
            if ($cim == false) {
                $error = true;
            } else {
                $customerProfileId = $cim[0];
                $customerPaymentProfileId = $cim[1];
                $add_cim_profile = true;
                // charge the user for the "membership kit?"
                /*if (isset($_POST['retail_kit']))
                {*/
                    $result_charge = charge_retail_kit($customerProfileId, $customerPaymentProfileId, $product_nid, $member_id, $dist->uid);
                    
                    if ($result_charge == false)
                    {   
                        $error = true;
                        $add_cim_profile = false;
                    }
                //}
            }
        }
        else if ($card_number == '5424000000000015')    // free card number 
        {
            drupal_set_message('Adding new FREE member...' , 'status');

            $customerProfileId = "FREE";
            $customerPaymentProfileId = "FREE";
            $add_cim_profile = true;
        }
        
        // do we add new cim profile?
        if ($add_cim_profile == true)
        {
        //  $query = "DELETE FROM member_cim WHERE member_id='".addslashes($member_id)."';";
            $result = db_query('DELETE FROM {member_cim} WHERE member_id=:a',array(':a'=>$member_id));
                      
        //  $query = "INSERT INTO member_cim VALUES(NULL, '".addslashes($member_id)."', '".addslashes($customerProfileId)."', '".addslashes($customerPaymentProfileId)."');";
            $result = db_query('INSERT INTO {member_cim} VALUES(NULL,:a,:b,:c)',
                    array(':a'=>$member_id,':b'=>$customerProfileId,':c'=>$customerPaymentProfileId));
        }

        // only carry on if no cc error
        if ($error == false)
        {
            // email the user with access details
            $params = array(
                '!firstname' => $first_name,
                '!lastname' => $last_name,
                '!member_id' => $member_id,
            );
            
            drupal_mail('ncn_claims_manager', 'id_to_member', $contactemail, language_default(), $params);
            
            // insert into database
            if ( $assign_whose == 'distributor' ) {
                $owner = $_POST['distributor'];
            } else {
                $owner = $_POST['associate'];   // * add this code for associate role *
            }
            
            $result = db_query('DELETE FROM {member_id_pool} WHERE member_id=:a',array(':a'=>$member_id));
            //$am_uid : account manager uid
            /*$query = "INSERT INTO member_id_pool VALUES(NULL, \"".addslashes($member_id)."\", 1, \"".addslashes($first_name)."\", \"".addslashes($last_name)."\", \"".addslashes($legalname)."\", \"".
                    addslashes($country)."\", \"".addslashes($address)."\", \"".addslashes($city)."\", \"".addslashes($state)."\", \"".addslashes($zip)."\", \"".addslashes($officephone)."\", \"". addslashes($mobilephone)."\", \"".
                    addslashes($contactemail)."\", \"".addslashes($distributor_name)."\", \"".addslashes($sales_rep)."\", \"".addslashes($security_code)."\", $owner, $am_uid, 0, 0, 0, $member_type ,$signup_fee_type, ".date('U')." )";
            */
            // $query = "INSERT INTO member_id_pool VALUES(NULL, '".addslashes($member_id)."', 1, '".addslashes($first_name)."', '".addslashes($last_name)."', '".addslashes($legalname)."', '".addslashes($country)."', '".addslashes($address)."', '".addslashes($city)."', '".addslashes($state)."', '".addslashes($zip)."', '".addslashes($officephone)."', '". addslashes($mobilephone)."', '".addslashes($contactemail)."', '".addslashes($distributor_name)."', '".addslashes($sales_rep)."', '".addslashes($security_code)."', $owner, $am_uid, 0, 0, 0, $member_type ,$signup_fee_type, ".date('U')." )";
            $result = db_query('INSERT INTO {member_id_pool} VALUES(NULL,:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:k1,:l,:m,:n,:o,:p,:q,:r,:s,:t,:u,:v,:w)',
                        array(':a'=>$member_id,
                              ':b'=>1,
                              ':c'=>$first_name,
                              ':d'=>$last_name,
                              ':e'=>$legalname,
                              ':f'=>$country,
                              ':g'=>$address,
                              ':h'=>$city,
                              ':i'=>$state,
                              ':j'=>$zip,
                              ':k'=>$officephone,
                              ':k1'=>$mobilephone,
                              ':l'=>$contactemail,
                              ':m'=>$distributor_name,
                              ':n'=>$sales_rep,
                              ':o'=>$security_code,
                              ':p'=>$owner,
                              ':q'=>$am_uid,
                              ':r'=>0,                      // used
                              ':s'=>0,                      // grant_subuser
                              ':t'=>0,                      // next_payment_date
                              ':u'=>$member_type ,
                              ':v'=>$signup_fee_type,
                              ':w'=>date('U')));

            if ($found_unused == 0)
            {
                drupal_set_message('Successfully added new member.' , 'status');
                //** NCN Gold Member 1st Special
                if ($_POST['retail_kit'] == SKU_SUF_GMS_1ST) {
                    ncn_admin_insert_member_first_free($member_id);
                }
                //** NCN Gold Demo Member
                else if ($_POST['retail_kit'] == SKU_SUF_G_DEMO) {
                    ncn_admin_insert_gold_member_demo($member_id);
                }
                if (module_exists('ncn_capsulecrm')) {
                    ncn_capsulecrm_create_member($member_id);
                }
            }
            else
            {   
                drupal_set_message('Overwriting existing member id#:'.$member_id , 'status');
            }
            
            $_POST = array();
            //unset($_POST);  // clear the form
        }
    }
    // duplicate member id
    else
    {
        drupal_set_message('That member id# is already in the database/being used.' , 'error');
    }
}

function ncn_admin_render_member_type($member_type) {
    ?>
    <select id="member_type" name="member_type">
        <option value="0" <?php if($member_type==MT_GOLD)       {echo "selected";} ?> >NCN Gold Member</option>
        <option value="1" <?php if($member_type==MT_SILVER)     {echo "selected";} ?> >NCN Silver Member</option>
        <option value="2" <?php if($member_type==MT_GOLD_LITE)  {echo "selected";} ?> >NCN Gold Lite Member</option>
        <option value="3" <?php if($member_type==MT_COACH_ON_CALL){echo "selected";} ?> >NCN Coach on Call Member</option>
        <option value="4" <?php if($member_type==MT_GOLD_COACH) {echo "selected";} ?> >NCN Gold Coach Member</option>
        <option value="5" <?php if($member_type==MT_CSI)        {echo "selected";} ?> >CSI Member</option>
    </select>
    <?php
}

/**
*/

function ncn_admin_draw_signup_fee_select_NEW($name, $default, $mode='') {
    // #options

    $payments = array();
    $pnode = node_load(56); $pnode->data = unserialize($pnode->data);
    $payments['56'] = $pnode;   // NCN Retail Kit ($329)
    $pnode = node_load(57); $pnode->data = unserialize($pnode->data);
    $payments['57'] = $pnode;   // CSI Sign Up ($0)
    $pnode = node_load(58); $pnode->data = unserialize($pnode->data);
    $payments['58'] = $pnode;   // NCN Retail Kit Options($149)
    $pnode = node_load(59); $pnode->data = unserialize($pnode->data);
    $payments['59'] = $pnode;   // NCN Premium Kit Options($249)
    /* NCN GOLD MEMBER SPECIAL  */
    $pnode = node_load(64); $pnode->data = unserialize($pnode->data);
    $payments['64'] = $pnode;   // NCN Premium Kit Options($99)

    // only for admin panel
    if ($mode=='') {
        $pnode = node_load(68); $pnode->data = unserialize($pnode->data);
        $payments['68'] = $pnode;   // NCN Gold First Special($0)

        $pnode = node_load(79); $pnode->data = unserialize($pnode->data);
        $payments['79'] = $pnode;   // NCN Gold Member Demo($0)
    }

    $pnode = node_load(168);    $pnode->data = unserialize($pnode->data);
    $payments['168'] = $pnode;  // NCN Gold Member Assoicate Special($49)

    /* NCN Gold Coach Member*/
    $pnode = node_load(157);    $pnode->data = unserialize($pnode->data);
    $payments['157'] = $pnode;  // NCN Gold Coach ($153.90)

    /* NCN Coach on Call  */
    $pnode = node_load(158);    $pnode->data = unserialize($pnode->data);
    $payments['158'] = $pnode;  // NCN Gold Coach ($54.90)

    /* <!-- <input type="checkbox" name="retail_kit_329"> <?= $pnode->title; ?> ($<?= number_format($pnode->sell_price, 2); ?>) --> */
    //ob_start();
    $select_field = "<select id=\"$name\" name=\"$name\"> \r\n";
    foreach ($payments as $key => $product) {
        $selected = ($key==$default)? "selected" : "";
        $select_field .= "<option value=\"$key\" $selected >".$product->title." ($".number_format($product->sell_price, 2).")</option>";
    }
    $select_field .= "</select> \r\n";
    //$select_field = ob_get_contents();
    //ob_end_clean();

    return $select_field;
}

function ncn_admin_draw_signup_fee_select($name, $default, $mode='') {
    // #options

    $payments = array();
    $pnode = node_load(SKU_RETAIL_KIT);
    $pnode->data = unserialize($pnode->data);
    $payments[SKU_RETAIL_KIT] = $pnode;   // NCN Retail Kit ($329)
    $pnode = node_load(SKU_CSI_SIGN_UP); $pnode->data = unserialize($pnode->data);
    $payments[SKU_CSI_SIGN_UP] = $pnode;   // CSI Sign Up ($0)
    $pnode = node_load(SKU_NCN_RETAIL_KIT_OPT); $pnode->data = unserialize($pnode->data);
    $payments[SKU_NCN_RETAIL_KIT_OPT] = $pnode;   // NCN Retail Kit Options($149)
    $pnode = node_load(SKU_NCN_PREMIUM_KIT_OPT); $pnode->data = unserialize($pnode->data);
    $payments[SKU_NCN_PREMIUM_KIT_OPT] = $pnode;   // NCN Premium Kit Options($249)
    /* NCN GOLD MEMBER SPECIAL  */
    $pnode = node_load(SKU_SUF_GMS); $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_GMS] = $pnode;   // NCN Premium Kit Options($99)
    
    // only for admin panel
    if ($mode=='') {
        $pnode = node_load(SKU_SUF_GMS_1ST); $pnode->data = unserialize($pnode->data);
        $payments[SKU_SUF_GMS_1ST] = $pnode;   // NCN Gold First Special($0)
        
        $pnode = node_load(SKU_SUF_G_DEMO); $pnode->data = unserialize($pnode->data);
        $payments[SKU_SUF_G_DEMO] = $pnode;   // NCN Gold Member Demo($0)
    }
    
    $pnode = node_load(SKU_SUF_GMAS); $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_GMAS] = $pnode;   // NCN Gold Member Assoicate Special($49)
    
    /* NCN Gold Coach Member*/
    $pnode = node_load(SKU_SUF_GC);    $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_GC] = $pnode;  // NCN Gold Coach ($153.90)
    
    /* NCN Coach on Call  */
    $pnode = node_load(SKU_SUF_COC);    $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_COC] = $pnode;  // NCN Gold Coach ($54.90)

    /* <!-- <input type="checkbox" name="retail_kit_329"> <?= $pnode->title; ?> ($<?= number_format($pnode->sell_price, 2); ?>) --> */
    $select_field = "<select id=\"$name\" name=\"$name\"> \r\n";
    foreach ($payments as $key => $product) {
        $selected = ($key==$default)? "selected" : "";
        $select_field .= "<option value=\"$key\" $selected >".$product->title." ($".number_format($product->sell_price, 2).")</option>";
    }
    $select_field .= "</select> \r\n";
    
    return $select_field;
}

/**
*/
function get_signup_fee_type_NEW($product_nid) {
    $fee_type = 0; // charged All.
    if ( ($product_nid == 58) ||        //NCN Retail Kit Options - $149
        ($product_nid == 59) ||         //NCN Premium Kit Options - $249
        ($product_nid == 60) )          //NCN Membership Kit Options - $99
    {
        $fee_type = 3; // charged Partially. for 3 months ($77)
    }
    return $fee_type;
}

function get_signup_fee_type($product_nid) {
    $fee_type = 0; // charged All.
    if ( ($product_nid == SKU_NCN_RETAIL_KIT_OPT) ||        //NCN Retail Kit Options - $149
         ($product_nid == SKU_NCN_PREMIUM_KIT_OPT) ||       //NCN Premium Kit Options - $249
         ($product_nid == SKU_SUF_DP) )                     //NCN Membership Kit Options - $99
    {
        $fee_type = 3; // charged Partially. for 3 months ($77)
    }
    return $fee_type;
}

function ncn_admin_member_download_pool_action() {
    header('Content-type: application/pdf');
    header('Content-Disposition: attachment; filename="member_ids_pool.csv"');

    echo "member id,first name,last name,address,city,state,zip,contact email,office phone,distibutor,sales rep\n";

    // $query = "SELECT * FROM member_id_pool ORDER BY member_id ASC;";
    $result = db_query('SELECT * FROM {member_id_pool} ORDER BY member_id ASC');
    $row_count = $result->rowCount();
    
    for ($i=0; $i<$row_count; $i++)
    {
        $row = $result->fetchAssoc();
        echo $row['member_id'].",".$row['first_name'].",".$row['last_name'].",".$row['address'].",".$row['city'].",".$row['state'].",".$row['zip'].",".$row['contactemail'].",".$row['officephone'].",".$row['distributor'].",".$row['sales_rep']."\n"; 
    }
    exit;
}

function ncn_admin_member_upload_ids_action() {
    // check for upload error
    if ($_FILES['ids']['error'] == 0)
    {
        $lines = file($_FILES['ids']['tmp_name']);
        $count = 0;
        $duplicates = 0;
        unset($lines[0]);
        
        foreach ($lines as $line)
        {
            $line =  preg_replace("/[^a-zA-Z0-9,]/", "", trim($line));
            
            if ($line != '')
            {
                $parts = explode(',', $line);

            //  $query = "SELECT * FROM member_id_pool WHERE member_id=\"".AddSlashes($parts[0])."\";";
                $result = db_query('SELECT * FROM {member_id_pool} WHERE member_id=:a',
                    array(':a'=>$parts[0]));
                $row_count = $result->rowCount();
                
                if ($row_count == 0)
                {
                    //$query = "INSERT INTO member_id_pool VALUES(NULL, \"".AddSlashes($parts[0])."\", \"".AddSlashes($parts[1])."\", \"".AddSlashes($parts[2])."\", ".$_POST['distributor'].", 0, 0, 0, $member_type ,$signup_fee_type, ".date('U')." )";
                    //$result = db_query($query);
                    $count++;
                }
                else
                {   $duplicates += 1;   } 
            }
            
        }

        // success message              
        drupal_set_message($count.' Member IDs added. '.$duplicates.' duplicates/previously used were ignored.' , 'status');
    }
    // an upload error occurred
    else
    {
        drupal_set_message('There was a problem with this upload. Please re-try.' , 'error');
    }
}

/*
 * Send Activation Mail, When activating member
 * @param:  $key - register_pending_approval (only one)
 */
 
function send_activation_mail($to, $language, $key, $params) {
    $variables = user_mail_tokens($params['account'], (array)$language, array());
    if(is_null($variables)){
        $variables = array();
    }   
    $subject = _user_mail_text($key .'_subject', $language, $variables);
    $msg     = _user_mail_text($key .'_body', $language, $variables);
    
    watchdog('activation_mail', 'op:!op, subject: !subject', array('!op'=> $key, '!subject'=>$subject));
    // mail headers
    $email = variable_get('site_mail', ini_get('sendmail_from'));
    $_admin = user_load(1);
    $headers =  'From: '.$email."\r\n".
                'Reply-To: '.$email."\r\n".
                "BCC: ".$_admin->mail . "\r\n" .
                'X-Mailer: PHP/'.phpversion();
                
    $attaches = array();
    $attache_names = array();
    
    $_user = user_load($params['account']->uid);
    profile_user_load(array($_user->uid => $_user));
    $member_type = get_member_type($_user);
    
    $node_title='Activation Mail (Gold)';
    $node_id = 55;
    if ($member_type == 0) {    // Gold Member
        $node_title='Activation Mail (Gold)';
        $node_id = 55;
    } else if ($member_type == 1) {
        $node_title='Activation Mail (Silver)';
        $node_id = 56;
    } else if ($member_type == 2) {
        $node_title='Activation Mail (Gold)';
        $node_id = 55;
    } else if ($member_type == 3) {
        $node_title='Activation Mail (Gold)';
        $node_id = 55;
    } else if ($member_type == 4) {
        $node_title='Activation Mail (Gold)';
        $node_id = 55;
    } else if ($member_type == 5) {
        $node_title='Activation Mail (Gold)';
        $node_id = 55;
    }
    
    $result = db_query('SELECT * FROM {node} WHERE type=:a AND nid=:b',array(':a'=>'attached_documents',':b'=>$node_id));
    if ($result->rowCount()>0) 
    {
        foreach($result as $row)
        {
            $row = (array)$row;
            $node = node_load($row['nid']);
        }
        
        $index = 0;
        $atchfiles = array_pop($node->field_attach_docs);
        foreach ($atchfiles as $file)
        {
            $file['filepath'] = str_replace("public://","sites/default/files/",$file['uri']);
            $attaches['doc-'.$index] = $file['filepath'];
            $attache_names['doc-'.$index] = $file['filename'];
            $attach_mail_data[$file['description']] = array('filepath'=>$file['filepath']);
            $index++;
        }
    }
    
    $_params = array('!name'=>($_user->profile_firstname." ".$_user->profile_lastname) ,'!docs'=>$attach_mail_data);
    drupal_mail('ncn_admin', 'create_user', $to, $language, $_params);
    return; //---------------------------
    
    // if (empty($attaches)) {
    //     drupal_mail('user', $key, $to, $language, $params, $email);
        
    //     /*$subject = "Acitvation Test Mail";
    //     $to      = "elight.haha@gmail.com";
    //     drupal_mail('user', $key, $to, $language, $params);*/
    // } else {
    //     send_mail_attached_file($to, $subject, $msg, $attaches, $attache_names, $headers);
    
    //     /*$subject = "Acitvation Test Mail";
    //     $to      = "elight.haha@gmail.com";
    //     send_mail_attached_file($to, $subject, $msg, $attaches, $attache_names, $headers);*/
    // }
}


//------------------------------------------------------------------------------
function update_username($uid, $new_username) {
    if (trim($new_username) == '') {
        drupal_set_message("UserName shouldn't be empty.", "error");
        return FALSE;
    }
    
    // validation
    $res = db_query('SELECT COUNT(*) FROM {users} WHERE uid != :d AND LOWER(name) = LOWER(:s)',array(':d'=>$uid,':s'=>$new_username))->fetchField();
//    var_dump($res);
//    exit;
    if (!$uid || user_access('administer users')) {
        if ($error = user_validate_name($new_username)) {
            drupal_set_message('User Name is invalid', 'error');
            return FALSE;
        }

        else if($res>0)
        {
            drupal_set_message( t('The name :name is already taken.', array(':name' => $new_username)), 'error' );
            return FALSE;
        }
    }
    
    // submit
    $account = user_load($uid);
    $name =  strtolower($new_username);

    $success = db_query('UPDATE {users} SET name=:a WHERE uid = :d', array(':a'=>$name,':d'=>$account->uid));
    if (!$success) {
        drupal_set_message("Failed to change username", "error");
        // The query failed - better to abort the save than risk further data loss.
        return FALSE;
    }
    
    // Refresh user object.
    $user = user_load(array('uid' => $account->uid));
    
    $category = 'account';
    user_module_invoke('after_update', $array, $user, $category);
    return TRUE;
}

function ncn_admin_edit_user_extra_profile($uid) {
    
    ob_start();
    user_edit_extra_profile($uid);
    $output = ob_get_contents();
    ob_end_clean();
    return $output;
}
// edit extra profile fields
function user_edit_extra_profile($uid, $in_admin=true)
{
    $title = "";
    $user_profile_url = 'account/my-account.html';  
    if ($in_admin) {
        $user_profile_url = 'admin/config/ncn_create_user/all_user_list/edit_user/'.$uid;
        $title = "Edit Extra Profile";
    }
    $user_profile_url = base_path().$user_profile_url;
    echo "<div style='width: 100%;' class='clearfix'>
                <h2>$title<div style='float: right; font-size: 14px; font-weight: normal;'><a href='$user_profile_url'>Back</a></div></h2>
                </div>";
    echo "<div id='edit_extra_profile_wrapper' style='margin-top: 10px;'>";
    echo display_user_extra_profiles($uid);
    echo "</div>";
}

function display_user_extra_profiles($uid) {
    ob_start();
    $user = user_load($uid);
    ?>
    <style>
        #edit-profile-taxnumber-wrapper, .form-item-profile-taxnumber{display:<?php echo ($user->profile_taxstatus=='1' ? 'block':'none'); ?>;}
        .form-item-profile-question-service {display:none;}
    </style>
    <script type="text/javascript">
        jQuery(document).ready(function(){
            jQuery("#edit-profile-taxstatus").click(function(){jQuery('.form-item-profile-taxnumber').slideToggle('slow');});
        });
    </script>
    <?php
    $content = ob_get_contents();
    ob_end_clean();
    
    $form = drupal_get_form('user_extra_profile_form'); 
    
    /*echo "<pre>";
    print_r($form);
    echo "</pre>";*/
    
    $content .= drupal_render($form);
    
    /*$form_errors = form_get_errors();
    if ( $form_errors ) {
        if (isset($form_errors['next-confirm'])) {
            //watchdog('trace-1', 'Here is Confirm Page'.$form_erros['next-confirm'], array());
            $content = "<div id=\"user_register_form\" style=\"display: none;\">$content</div>";
            $javascript = 
                "\r\t<script type='text/javascript'> 
                $().ready(function() {
                    $('#edit-user-confirm-page').val('1');
                });
                </script>";
            //$content .=$javascript;
            $confirm_page = get_user_confirm_page();
            $content .= $confirm_page;
        }
    }*/
    
    return $content;
}

function user_extra_profile_form($form, &$form_state) {
    //global $user;
    if (arg(0) == 'admin') {
        $uid = arg(4);
    } else {
        $uid = $GLOBALS['user']->uid;
    }
    $c_user = user_load($uid);

    module_load_include('pages.inc', 'user', 'user');
    
    //$form = user_edit_form($form_state, NULL, NULL, TRUE);    
    //$extra = _user_forms($null, NULL, NULL, 'register');
    
    $form = array();
    $extra = array();
    $form = drupal_get_form('user_profile_form',$c_user);
    
    $extraval = $c_user;
    if(isset($_POST['op'])){
        foreach($_POST as $key=>$val) {
            if (strpos($key, 'profile_') === 0) {
                $extraval->$key = $val;
            }
        }
    }

    $extraA = drupal_get_form('user_profile_form',$extraval,'Company Information');
    $extraB = drupal_get_form('user_profile_form',$extraval,'Operations Information');
    $extraC = drupal_get_form('user_profile_form',$extraval,'Principal Owners');
    $extra = array_merge($extraC,$extraB,$extraA);

    // patch in the firstname/lastname
    if (isset($_GET['id']))
    {
        $result = db_query('SELECT * FROM member_id_pool WHERE member_id=:mid',array(':mid'=>addslashes(base64_decode($_GET['id']))));
        if ($result->rowCount() > 0)
        {
            $row = $result->fetchAssoc();
            
            $groups['Company Information']['profile_firstname']['#default_value'] = $row['first_name'];
            $groups['Company Information']['profile_firstname']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_lastname']['#default_value'] = $row['last_name'];
            $groups['Company Information']['profile_lastname']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_legalname']['#default_value'] = $row['legalname'];
            $groups['Company Information']['profile_legalname']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_country']['#default_value'] = $row['country'];
            $groups['Company Information']['profile_country']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;
            
            $groups['Company Information']['profile_address']['#default_value'] = $row['address'];
            $groups['Company Information']['profile_address']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_city']['#default_value'] = $row['city'];
            $groups['Company Information']['profile_city']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_state']['#default_value'] = $row['state'];
            $groups['Company Information']['profile_state']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_zip']['#default_value'] = $row['zip'];
            $groups['Company Information']['profile_zip']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_officephone']['#default_value'] = $row['officephone'];
            $groups['Company Information']['profile_officephone']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;

            $groups['Company Information']['profile_contactemail']['#default_value'] = $row['contactemail'];
            $groups['Company Information']['profile_contactemail']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;
            
            // make others readonly
            if (trim($row['sales_rep']) != '')
            {
                $groups['Distributor']['profile_sales_rep']['#default_value'] = $row['sales_rep'];
                $groups['Distributor']['profile_sales_rep']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;
            }

            $groups['Distributor']['profile_distributor']['#default_value'] = $row['distributor'];
            $groups['Distributor']['profile_distributor']['#attributes'] = array('readonly' => 'readonly', 'style' => 'background-color:#EBEBE4;');;
            
            // has the administrator already set the credit card details?
            $result2 = db_query('SELECT * FROM member_cim WHERE member_id=:mid',array(':mid'=>addslashes(base64_decode($_GET['id']))));
            if ($result2->rowCount()!= 0)
            {
                $row2 = $result2->fetchAssoc();

                // check for free
                if ($row2['customerProfileId'] == "FREE")   
                {
                    // add hidden credit card details for free account
                    unset($groups['Credit Card Information']['profile_credit_card_type']);                  
                    unset($groups['Credit Card Information']['profile_credit_card_expiration']);                    
                    unset($groups['Credit Card Information']['profile_credit_card_security']);                  
                    unset($groups['Credit Card Information']['profile_credit_card_name']);

                    unset($groups['Credit Card Information']['#type']);
                    
                    $groups['Credit Card Information']['profile_credit_card_number']['#type'] = "hidden";                   
                    $groups['Credit Card Information']['profile_credit_card_number']['#default_value'] = "5424000000000015";                    
                }
                else
                {
                    unset($groups['Credit Card Information']);
                }
            }   // END cc profile already set
        }
    }
    
    // insert the default email address
    $form['account']['mail']['#default_value'] = $extra['Company Information']['profile_contactemail']['#default_value'];
    
    // Remove form_group around default fields if there are no other groups.
    if (!$extra) {
        foreach (array('name', 'mail', 'pass', 'status', 'roles', 'notify') as $key) {
            if (isset($form['account'][$key])) {
                $form[$key] = $form['account'][$key];
            }
        }
        unset($form['account']);
    }
    else {
        $form = array_merge($form, $extra);
    }
    
    $_user = user_load($uid);
    // update form
    $form['huser_id'] = array(
        '#type' => 'hidden', 
        '#default_value' => $uid,
    );
    
    unset($form['account']);
    unset($form['picture']);
    unset($form['timezone']);
    unset($form['Distributor']);
    unset($form['Credit Card Information']);
    unset($form['Company Information']['profile_firstname']);
    unset($form['Company Information']['profile_lastname']);
    unset($form['Company Information']['profile_legalname']);
    unset($form['Company Information']['profile_country']);
    unset($form['Company Information']['profile_address']);
    unset($form['Company Information']['profile_city']);
    unset($form['Company Information']['profile_state']);
    unset($form['Company Information']['profile_zip']); 
    unset($form['Company Information']['profile_contactemail']);
    unset($form['Company Information']['profile_memberid']);
    unset($form['Company Information']['profile_fblocked']);
    unset($form['Company Information']['profile_blocked_time']);
    unset($form['Principal Owners']['profile_po_zip']);
    
    foreach ($form as $fieldset => $fs_part) {
        if (is_array($fs_part)) {
            foreach ($fs_part as $key => $profile_data) {
                if (strpos($key, 'profile_') === 0) {
                    $form[$fieldset][$key]['#default_value'] = (isset($_user->$key) ? $_user->$key : '');
                }
            }
        }
    }   
    
  $form['actions']['submit']['#value'] = t('Update');
  $form['#submit'] = array();
  $form['#submit'][] = 'user_extra_profile_form_submit';
  $form['#validate'] = array();
  $form['#validate'][] = 'user_extra_profile_form_validate';
  return $form;
}


function getListOfProfileFields($fieldsArray){
    
    $query = db_select('profile_field');
    $query->condition('name',$fieldsArray, 'IN');
    if (!user_access('administer users')) {
        $query->condition('visibility', PROFILE_HIDDEN, '<>');
    }
    return $query
    ->fields('profile_field')
    ->orderBy('category', 'ASC')
    ->orderBy('weight', 'ASC')
    ->execute();
}

function user_extra_profile_form_validate($form, &$form_state) {

    if($form['#form_id'] == 'user_register')
    {
        $account = $form['#user'];

        // Trim whitespace from mail, to prevent confusing 'e-mail not valid'
        // warnings often caused by cutting and pasting.
        $mail = trim($form_state['values']['mail']);
        form_set_value($form['account']['mail'], $mail, $form_state);

        // Validate the e-mail address, and check if it is taken by an existing user.
        if ($error = user_validate_mail($form_state['values']['mail'])) {
            form_set_error('mail', $error);
        }
        elseif ((bool) db_select('users')->fields('users', array('uid'))->condition('mail', db_like($form_state['values']['mail']), 'LIKE')->range(0, 1)->execute()->fetchField()) {
            // Format error message dependent on whether the user is logged in or not.
            if ($GLOBALS['user']->uid) {
                form_set_error('mail', t('The e-mail address %email is already taken.', array('%email' => $form_state['values']['mail'])));
            }
            else {
                form_set_error('mail', t('The e-mail address %email is already registered. <a href="@password">Have you forgotten your password?</a>', array('%email' => $form_state['values']['mail'], '@password' => url('user/password'))));
            }
        }

    }

    $form_state['values'] = $form_state['input'];
    $values = $form_state['values'];
    $keys = array_keys($values);
    $result = getListOfProfileFields($keys);

    
    foreach ($result as $field) {
        if (isset($form_state['values'][$field->name]) && !empty($form_state['values'][$field->name])) {
          if ($field->type == 'url' && !valid_url($form_state['values'][$field->name], TRUE)) {
            form_set_error($field->name, t('The value provided for %field is not a valid URL.', array('%field' => $field->title)));
          }
        }
        elseif ($field->required && !user_access('administer users')) {
          form_set_error($field->name, t('The field %field is required.', array('%field' => $field->title)));
        }
    }
} 


function user_extra_profile_form_submit($form, $form_state) {
    //global $user;
    $data = $form_state['values'];
    $uid = $data['huser_id'];
    
    $newinfo = user_load($uid);
    
    $account = new stdClass();
    $account->uid = $uid;
    
    foreach($data as $key=>$val) {
        if (strpos($key, 'profile_') === 0) {
            $newinfo->$key = $val;
        }
    }
    if(!isset($data['profile_taxstatus']))
    {
        $newinfo->profile_taxstatus = 0;
    }
    //print_r($newinfo); exit;

    $edit = (array)$newinfo;
    profile_save_profile($edit, $account, 'Company Information');
    profile_save_profile($edit, $account, 'Operations Information');
    profile_save_profile($edit, $account, 'Principal Owners');
    
    drupal_set_message(t('Update extra profiles, successfully.'));
}

// used here: http://www.netclaimsnow.com/admin/user/user/list
function user_edit_new($in_admin = true, $uid = false)
{
    drupal_add_js(drupal_get_path('module', 'ncn_admin') . '/ncn_admin.js');
    // we are running this frunction from admin 
    if ($in_admin == true)
    {
        $user_id = arg(4);

        // check url supplied user id is valid
        if (!is_numeric($user_id))
        {   Header("Location:admin/user/user/list");exit;       }
                
        // load the user
        $user = user_load($user_id);
        
        if (!$user)     // check user is valid
        {   Header("Location:admin/user/user/list");exit;       }

        // set default page title
        $title = "Edit User: ".$user->profile_firstname." ".$user->profile_lastname;
        
    }
    // we are running this function from users account
    else
    {   
        $user_id = $uid;    
        $user = user_load($user_id);    // this will always work in users account

        // set default page title
        $title = "Edit My Profile";
    }

    // ---- function ----
    $tfunction = (isset($_POST['tfunction']) ? $_POST['tfunction'] : '');
    switch ($tfunction)
    {
        case "update_profile":
            
            $username_changed = false;
            $email_changed = false;
            $pass_changed = false;
            
            // has user updated their email address?
            if ($user->name != $_POST['username'])
            {
                if (update_username($user->uid, $_POST['username'])) {
                    drupal_set_message('username has been updated');
                    $username_changed = true;
                }
            }
            
            if ($user->mail != $_POST['mail'])
            {
                // check new email address is valid
                if ($error = user_validate_mail($_POST['mail']))
                {
                    drupal_set_message($error, 'error');    // email failed verfication 
                }
                // email was valid, so update in the database and flag to send new welcome email 
                else
                {
                    // save new email in database
                    $query = 'UPDATE `users` SET `mail` = :mail WHERE uid = :uid';
                    $result = db_query($query,array(':mail'=>addslashes($_POST['mail']),':uid'=>$user_id));
                    
                    // flag for confirmation email
                    $email_changed = true;
                    
                    // success message
                    drupal_set_message(t("Email address has been updated."), 'status');
                }
            }
            
            // has user updated the password?
            if (($_POST['pass'] != '') || ($_POST['pass2'] != ''))
            {
                // check match
                if ($_POST['pass'] != $_POST['pass2'])
                {
                    drupal_set_message(t("Passwords do not match."), 'error');
                }
                else
                if (strlen($_POST['pass']) < 5)
                {
                    drupal_set_message(t("Password must be at least 5 characters."), 'error');
                }
                // all good, so update it 
                else
                {
                    // update database with new password 
                    require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
                    $encrypted_pass = user_hash_password(trim($_POST['pass']));
                    
                //  $query = "UPDATE `users` SET `pass`=\"".AddSlashes($encrypted_pass)."\" WHERE uid=".$user_id;
                    $result = db_query('UPDATE {users} SET pass=:a WHERE uid=:b',
                        array(':a'=>$encrypted_pass,':b'=>$user_id));
                    
                    // flag for confirmation email
                    $pass_changed = true;

                    // success message
                    drupal_set_message(t("Password has been updated."), 'status');
                }
                
            }
            
            // update all other details
            /*update_profile_var($user->uid, 'profile_firstname', $_POST['profile_firstname']);
            update_profile_var($user->uid, 'profile_lastname', $_POST['profile_lastname']);
            update_profile_var($user->uid, 'profile_legalname', $_POST['profile_legalname']);
            update_profile_var($user->uid, 'profile_country', $_POST['profile_country']);
            update_profile_var($user->uid, 'profile_address', $_POST['profile_address']);
            update_profile_var($user->uid, 'profile_city', $_POST['profile_city']);
            update_profile_var($user->uid, 'profile_state', $_POST['profile_state']);
            update_profile_var($user->uid, 'profile_zip', $_POST['profile_zip']);*/
            
            $user->profile_firstname = $_POST['profile_firstname'];
            $user->profile_lastname = $_POST['profile_lastname'];
            $user->profile_legalname = $_POST['profile_legalname'];
            $user->profile_country = $_POST['profile_country'];
            $user->profile_address = $_POST['profile_address'];
            $user->profile_city = $_POST['profile_city'];
            $user->profile_state = $_POST['profile_state'];
            $user->profile_zip = $_POST['profile_zip'];
            
            $account = new stdClass;
            $account->uid = $user->uid;
            
            $edit = (array)$user;
            /*echo "<pre>";             
            print_r($edit);
            exit;*/
            
            profile_save_profile($edit, $account, 'Company Information');
            
            // check for no changes
            //if ($email_changed || $pass_changed || $username_changed)
            //{
                drupal_set_message(t("Profile has been updated."), 'status');
            //}

            if (isset($user->profile_memberid)&&is_member($user->profile_memberid)) {
                $result = db_query("UPDATE {member_id_pool} SET 
                                   first_name=:fn, last_name=:ln, legalname=:lgn, country=:cn, address=:ad, city=:ct, state=:st, zip=:zp WHERE member_id=:mid", 
                                   array(':fn'=>$_POST['profile_firstname'],':ln'=>$_POST['profile_lastname'],':lgn'=>$_POST['profile_legalname'],
                                         ':cn'=>$_POST['profile_country'],':ad'=>$_POST['profile_address'],':ct'=>$_POST['profile_city'], 
                                         ':st'=>$_POST['profile_state'],':zp'=>$_POST['profile_zip'],':mid'=>$user->profile_memberid)
                                  );
            }
            // we need to reload user
            $user = user_load($user_id);
        break;
        case "upload_member_logo":
            ncn_admin_upload_member_logo($_REQUEST['member_id']);
            break;
        case "remove_member_logo":
            ncn_admin_remove_member_logo($_REQUEST['member_id']);
            break;
        case "update_user_role":
            if (ncn_admin_update_user_role($user, $_POST['user_role'])) {
                drupal_set_message(t("User role has been updated."), 'status');
                $user = user_load($user_id);
            } else {
                drupal_set_message(t("Failed to update user role."), 'error');
            }
            break;
        case "update_dist_asso":
            $owner_id = 0; 
            if (isset($user->profile_memberid)) {
                $owner_id = get_owner($user->profile_memberid);
                $new_type = $_POST['dist_asso'];
                $new_ownerid = $_POST[$new_type];
                if ($owner_id != $new_ownerid) {
                    ///////////////////////////////////////////////////////
                //  $query = "UPDATE member_id_pool SET owner=$new_ownerid WHERE member_id ='".$user->profile_memberid."'";
                    $result = db_query('UPDATE {member_id_pool} SET owner=:a WHERE member_id =:b',
                        array(':a'=>$new_ownerid,':b'=>$user->profile_memberid));
                    if ($result) {
                        drupal_set_message("Updated associated $new_type successfully.");
                        ncn_report_change_owner($user->profile_memberid, $owner_id, $new_ownerid);
                    } else {
                        drupal_set_message("Failed to update associated $new_type", "error");
                        break;
                    }
                }
            }
            break;
        case "update_account_manager":
            $am_uid = $_POST['am_uid'];
            if (!ncn_admin_assign_member_to_am($user->profile_memberid, $am_uid)) {
                drupal_set_message(t('Failed to reassign. Please report to web master.'), 'error');
            } else {
                drupal_set_message(t('Reassigned to account manager, successfully.'));
            }
            
            break;
        case "change_report_pass":
            if ( ncn_dist_change_report_pass() ) {  
            
            }
            break;
      //  $res = db_query('SELECT COUNT(*) FROM {member_cim} WHERE member_id=:a',array(':a'=>$user->profile_memberid))->fetchField();
        case "update_practicepay_profile":
            if (is_distributor($user))  // for distributors
            {
                $query = db_query('UPDATE {member_distributor_cim} SET customerProfileId=:a, customerPaymentProfileId=:b
                WHERE username=:c',array(':a'=>$_POST['customerProfileId'],':b'=>$_POST['customerPaymentProfileId'],
                    ':c'=>$user->name));
            }
            else    // for normal users
            {
                if ((db_query('SELECT COUNT(*) FROM {member_cim} WHERE member_id=:a',array(':a'=>$user->profile_memberid))->fetchField())==0) {
                    $query = db_query('INSERT INTO {member_cim} (member_id, customerProfileId, customerPaymentProfileId)
                    VALUES(:a,:b,:c)',array(':a'=>$user->profile_memberid,':b'=>$_POST['customerProfileId'],
                        ':c'=>$_POST['customerPaymentProfileId']));
                } else {
                    $query = db_query('UPDATE {member_cim} SET customerProfileId=:a, customerPaymentProfileId=:b
                    WHERE member_id=:c',array(':a'=>$_POST['customerProfileId'],':b'=>$_POST['customerPaymentProfileId'],
                        ':c'=>$user->profile_memberid));
                }
            }
            
            $result = $query;   // do the sql update
            drupal_set_message(t("PracticePay Profile ID's have been updated in the database."), 'status');
            
        break;
        case "update_member_type":
      $p_member_type = $_POST['member_type'];
      $member = get_member_from_id($user->profile_memberid);
      if ($member['member_type'] == $p_member_type) { 
        break; 
      } else {
        ncn_user_insert_log_old_member_type($user->profile_memberid, $member['member_type'], $p_member_type, date('U'));
      }
            
            $query = db_query('UPDATE {member_id_pool} SET member_type =:a WHERE member_id=:b',
                array(':a'=>$p_member_type,':b'=>$user->profile_memberid));
            
            if ($p_member_type==5) {
                $member_type_name = 'CSI Member';
            } else if ($p_member_type==4) {
                $member_type_name = 'NCN Gold Coach Member';    
            } else if ($p_member_type==3) {
                $member_type_name = 'NCN Coach on Call Member';
            } else if ($p_member_type==2) {
                $member_type_name = 'NCN Gold Lite Member';
            } else if ($p_member_type==1) {
                $member_type_name = 'NCN Silver Member';
            } else {
                $member_type_name = 'NCN Gold Member';
            }
            
            $result = $query;
            if ($result) {
                drupal_set_message(t('Member Type updated to !member_type_name successfully', array('!member_type_name'=>$member_type_name)) );
            } else {
                drupal_set_message(t('Failed to update member type.'), 'error');
            }
        break;
    case "cancel_pause_member":
      $result = ncn_admin_cancel_pause_member($user->uid);
      $user = user_load($user->uid);
      break;
    case "unblock_member":
      $result = ncn_admin_unblock_member($user->uid);
      $user = user_load($user->uid);
      break;
    case "change_member_renewal_date":
      $result = ncn_user_change_member_renewal_day($user->profile_memberid, (int)($_POST['ncn_admin_member_renewal_date']));
      if ($result) {
        drupal_set_message("Success to change member renewal day");        
      }
      $user = user_load($user->uid);
      break;
        case "send_claim_reminder_mail": 
            $result = ncn_admin_send_reminder_mail($user->uid);
        break;
        case "grant_subuser" :
            $grant_subuser = $_POST['grant_subuser'];
            ncn_subuser_grant_creating_subuser($user->profile_memberid, $grant_subuser);
        break;
        case "convert_demo_to_regular":
            $result = db_query('UPDATE {member_gold_demo} SET status=0 WHERE member_id=:a AND expired=0 AND status=1 ORDER BY to_expired_time DESC',
                array(':a'=>$user->profile_memberid));
            if ($result) {
                drupal_set_message(t('Convert demo to regular, successfully.'));
            } else {
                drupal_set_message(t('Failed to convert demo to regular.'), 'error');
            }
        case "update_cc":
            
            /*
            // check for free
            if ($_POST['credit_card_number'] != '5424000000000015')
            {
            
                // add the user to cim
                $content =
                    "<?xml version=\"1.0\" encoding=\"utf-8\"?>" .
                    "<createCustomerProfileRequest xmlns=\"AnetApi/xml/v1/schema/AnetApiSchema.xsd\">" .
                    MerchantAuthenticationBlock().
                    "<profile>".
                    "<merchantCustomerId>" . get_unique_auth_profile_id() . "</merchantCustomerId>". // Your own identifier for the customer.
                    "<description>".$user->profile_firstname." ".$user->profile_lastname."</description>".
                    "<email>" . $user->mail . "</email>".
                    "</profile>".
                    "</createCustomerProfileRequest>";  // <?php
                
                $response = send_xml_request($content);
                $parsedresponse = parse_api_response($response);
                    
                $customerProfileId = $parsedresponse->customerProfileId;
            
            
                // try and add the payment profile
                $date = explode("/", $_POST['credit_card_expiration']);
                
                if (sizeof($date) == 2)
                {       $date = "20".$date[1]."-".$date[0];   }
                else
                {       $date = '';     }
                
                $content =
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>" .
                "<createCustomerPaymentProfileRequest xmlns=\"AnetApi/xml/v1/schema/AnetApiSchema.xsd\">" .
                MerchantAuthenticationBlock().
                "<customerProfileId>" . $customerProfileId . "</customerProfileId>".
                "<paymentProfile>".
                "<billTo>".
                 "<firstName>".$user->profile_firstname."</firstName>".
                 "<lastName>".$user->profile_lastname."</lastName>".
//               "<country>".$user->profile_country."</country>".
                 "<address>".$user->profile_address."</address>".
                 "<city>".$user->profile_city."</city>".
                 "<state>".$user->profile_state."</state>".
                 "<zip>".$user->profile_zip."</zip>".
                 "<phoneNumber>000-000-0000</phoneNumber>".
                "</billTo>".
                "<payment>".
                 "<creditCard>".
                  "<cardNumber>".$_POST['credit_card_number']."</cardNumber>".
                  "<expirationDate>".$date."</expirationDate>". // required format for API is YYYY-MM
                  "<cardCode>".$_POST['credit_card_security']."</cardCode>".
                 "</creditCard>".
                "</payment>".
                "</paymentProfile>".
                "<validationMode>liveMode</validationMode>". // or liveMode
                "</createCustomerPaymentProfileRequest>";   // <?php
            
                $response = send_xml_request($content);
                $parsedresponse = parse_api_response($response);
                $customerPaymentProfileId = $parsedresponse->customerPaymentProfileId;

                // error check
                if ($parsedresponse->messages->resultCode == 'Error')
                {
                    $error = "Credit/debit card details are invalid/expired.";
                    drupal_set_message($error, 'error');
                }
                else
                {
                    // success 
                    drupal_set_message(t("Payment profile updated with new credit card."), 'status');
                }

            }   // END check free
            else
            {
                // save this profile as free!
                $customerProfileId = "FREE";
                $customerPaymentProfileId = "FREE";
                
                drupal_set_message(t("Credit card updated to FREE."), "status");
            }       
            
        
            // update the payment profile id# in the database
            if (!isset($error))
            {
                // now update the actual card details in the profile
                //update_profile_var($user->uid, 'profile_credit_card_number', $_POST['credit_card_number']);
                //update_profile_var($user->uid, 'profile_credit_card_expiration', $_POST['credit_card_expiration']);
                update_profile_var($user->uid, 'profile_credit_card_security', $_POST['credit_card_security']);

                // switch for distributor/user
                if (is_distributor($user))
                {
                    $query = "UPDATE member_distributor_cim SET customerProfileId=\"".AddSlashes($customerProfileId)."\",customerPaymentProfileId=\"".AddSlashes($customerPaymentProfileId)."\" WHERE username=\"".AddSlashes($user->name)."\";";
                    $result = db_query($query);
                }
                else    // normal users
                {
                    $query = "UPDATE member_cim SET customerProfileId=\"".AddSlashes($customerProfileId)."\",customerPaymentProfileId=\"".AddSlashes($customerPaymentProfileId)."\" WHERE member_id=\"".AddSlashes($user->profile_memberid)."\";";
                    $result = db_query($query);
                }
                
            }   // END there were no errors
            */  
        if ($_POST['credit_card_security']!="") {
            update_profile_var($user->uid, 'profile_credit_card_security', $_POST['credit_card_security']);
        //  $query = "UPDATE member_id_pool SET security_code='".$_POST['credit_card_security']."' WHERE member_id='".$user->profile_memberid."'";
            $result = db_query('UPDATE {member_id_pool} SET security_code=:a  WHERE member_id=:b',
            array(':a'=>$_POST['credit_card_security'],':b'=>$user->profile_memberid));
            $user = user_load($user->uid);
        }
        break;
        case "update_org_party_id":
            $org_id = intval($_POST['org_id']);
            $person_id = intval($_POST['person_id']);
            if (ncn_capsulecrm_save_map_info($user->profile_memberid, $org_id, $person_id) ) {
        ncn_capsulecrm_set_user_detail_link($user->uid);
                drupal_set_message("Success to update capsulecrm party id");
            } else {
                drupal_set_message("Failed to update capsulecrm party id", 'error');
            }
        break;
    case "update_capsulecrm_user_token":
      $casulecrm_user_token = $_POST['casulecrm_user_token'];
      if (ncn_capsulecrm_token_save_user_token($user->uid, $casulecrm_user_token)) {
        drupal_set_message("Success to set capsulecrm user token");
      } else {
        drupal_set_message("Failed to set capsulecrm user token", 'error');
      }
    break;
    
    case "update_capsulecrm_account":
      $capsulecrm_account = $_POST['casulecrm_account'];
      if (ncn_capsulecrm_account_save_account($user->uid, $capsulecrm_account)) {
        drupal_set_message("Success to set capsulecrm account");
      } else {
        drupal_set_message("Failed to set capsulecrm account", 'error');
      }
    break;
    }
    
    // ---- show the form ----
    ?>
    <!-- <h2><?= $title; ?></h2> -->
    
    <fieldset class="edit_user_group">
    <?php if(isset($user->profile_memberid)&&is_member($user->profile_memberid)): ?>
    <?php 
        $extra_field_url = 'account/my-extra-profile.html';
        if (arg(0) == 'admin') {
            $extra_field_url = 'admin/config/ncn_create_user/edit_user_extra_profile/'.$user->uid;
        }
    ?>
        <div style="float: right; margin-top: 5px;"><a href="<?php echo base_path().$extra_field_url; ?>">Edit extra profiles</a></div>
    <?php endif;?>
    <h2 class="sub-title">Profile</h2>

    <p style="font-style: italic; font-size: 11px;">Password is optional.</p>
    <p>For example, if you only wish to update users email address, leave all other fields blank.</p>
    
    <form method="POST" id="update_profile_form">
    <input type="hidden" name="tfunction" value="update_profile">
    <table style="width:100%;">
        <tr>
            <td class="td-label">Username:</td>
            <td><input type="text" name="username" id="username" value="<?= $user->name; ?>" style="width:200px;" <?php if ($GLOBALS['user']->uid!=1 && !user_access('ncn_admin edit username')) {echo "readonly";} ?>></td>
        </tr>
        <!-- <tr>
            <td class="td-label">Name:</td>
            <td><input type="text" disabled="disabled" value="<?= $user->profile_firstname; ?> <?= $user->profile_firstname; ?>"  style="width:200px;"></td>
        </tr>
        <tr>
            <td class="td-label">Company:</td>
            <td><input type="text" disabled="disabled" value="<?= $user->profile_legalname; ?>"  style="width:200px;"></td>
        </tr> -->
        <tr>
            <td class="td-label">Email:</td>
            <td><input type="text" name="mail" value="<?= $user->mail; ?>"  style="width:200px;"></td>
        </tr>
        <tr>
            <td class="td-label">Password:</td>
            <td><input type="password" name="pass" value=""  style="width:200px;"></td>
        </tr>
        <tr>
            <td class="td-label">Confirm Password:</td>
            <td><input type="password" name="pass2" value=""  style="width:200px;"></td>
        </tr>
        
        <tr>
            <td class="td-label">First Name:</td>
            <td><input type="text" name="profile_firstname" value="<?= $user->profile_firstname; ?>">
        </tr>
        <tr>
            <td class="td-label">Last Name:</td>
            <td><input type="text" name="profile_lastname" value="<?= $user->profile_lastname; ?>">
        </tr>
        <tr>
            <td class="td-label">Company's Name:</td>
            <td><input type="text" name="profile_legalname" value="<?= $user->profile_legalname; ?>">
        </tr>
        <tr>
            <td class="td-label">Country:</td>
            <td><?php draw_countryform_part_sel('profile_country', false, false, $user->profile_country);?></td>
        </tr>
        <tr>
            <td class="td-label">Address:</td>
            <td><input type="text" name="profile_address" value="<?= $user->profile_address; ?>">
        </tr>
        <tr>
            <td class="td-label">City:</td>
            <td><input type="text" name="profile_city" value="<?= $user->profile_city; ?>">
        </tr>
        <tr>
            <td class="td-label">Province/State:</td>
            <td><?php draw_stateform_part_sel('profile_state', false, false, $user->profile_country, $user->profile_state);?></td>
        </tr>
        <tr>
            <td class="td-label">Zip:</td>
            <td><input type="text" name="profile_zip" value="<?= $user->profile_zip; ?>">
        </tr>
        
        <tr>
            <td class="td-label"></td>
            <td><a class="update-profile-btn" href="javascript:void(0)" onclick="update_profile_submit();">Update Profile</a></td>
        </tr>
    </table>
    </form>
    
    <?php if(isset($user->profile_memberid)&&is_member($user->profile_memberid)) : ?>
    <!-- Upload Member Logo-->
    <form id="upload_member_logo_form" method="POST" enctype="multipart/form-data">
        <input type="hidden" id="member_logo_tfunction" name="tfunction" value="upload_member_logo" />
        <input type="hidden" name="member_id" value="<?php echo $user->profile_memberid; ?>" />
        <table style="width:100%; margin-top: 30px;">
        <tr>
            <td class="td-label">Member's logo:</td>
            <td>
                    <?php if ($member_logo_path = ncn_admin_get_member_logo($user->profile_memberid)): ?>
                        <img src="<?php echo base_path().$member_logo_path; ?>" style="width: 300px"/> <br/>
                    <?php endif; ?>
                    <input type="file" size="25" accept="image/jpg, image/jpeg, image/gif, image/png" id="member_logo" name="files[member_logo]" /></td>
        </tr>
        <tr>
            <td class="td-label"></td>
            <td><a class="upload-logo-btn" href="javascript:void(0)" onclick="upload_member_logo();">Upload Member Logo</a>
                <?php if ($member_logo_path = ncn_admin_get_member_logo($user->profile_memberid)): ?>
                    <a class="remove-logo-btn" href="javascript:void(0)" onclick="remove_member_logo();" style="margin-left: 15px;">Remove Member Logo</a>
                <?php endif; ?>
            </td>
        </tr>
        </table>
    </form>
    <?php endif; ?>
    </fieldset>
    
    <?php if(isset($user->profile_memberid) && ncn_admin_is_gold_member_demo($user->profile_memberid)): ?>
    <?php
        $result = db_query('SELECT to_expired_time, num_claims FROM {member_gold_demo} WHERE member_id=:a AND expired=0 AND status=1 ORDER BY to_expired_time DESC',
            array(':a'=>$user->profile_memberid));
        if ($result) {
//          if ($row=db_fetch_array($result))
            foreach($result as $row)
            {
                $row = (array)$row;
                if($row)
                {
                    $expired_time = date('m/d/Y H:i', $row['to_expired_time']);
                    $num_claims = $row['num_claims'];
                }
            }
        }
    ?>
    <fieldset class="edit_user_group">
        <h2 class="sub-title">Gold Member Demo</h2>
        <form method="POST" id="convert_demo_to_regular">
        <input type="hidden" name="tfunction" value="convert_demo_to_regular">
        <table style="width:100%;">
            <tr>
                <td class="td-label">Expired Time:</td>
                <td>
                    <?php echo $expired_time; ?>
                </td>
            </tr>
            <tr>
                <td class="td-label">Created Free Claims:</td>
                <td>
                    <?php echo $num_claims; ?>
                </td>
            </tr>
            <tr><td colspan='2'>&nbsp;</td></tr>
            <tr>
                <td colspan='2'><input type="submit" value="Convert to Regular Member" /></td>
            </tr>
        </table>
        </form>
    </fieldset> 
    <?php endif; ?>
    
    <?php   // Role
        if ($in_admin && !(isset($user->profile_memberid)&&is_member($user->profile_memberid)) && !is_distributor($user) && !is_associate($user) && !is_subuser($user) ): 
    ?>
    <?php 
        $select_role = 1;
        foreach ($user->roles as $rid=>$role_name)
        {
            $select_role = $rid;
        }
    ?>
    <fieldset class="edit_user_group">
        <h2 class="sub-title">User Role</h2>
        <form method="POST" id="update_user_role">
        <input type="hidden" name="tfunction" value="update_user_role">
        <table style="width:100%;">
            <tr>
                <td class="td-label">Role:</td>
                <td>
                    <?php echo ncn_admin_render_select_role('user_role', $select_role, ''); ?>
                </td>
            </tr>
            <tr>
                <td colspan='2'><input type="submit" value="Save" /></td>
            </tr>
        </table>
        </form>
    </fieldset>
    
    <?php endif; // Role?>
    
    <?php if ($GLOBALS['user']->uid==1 || user_access('ncn capsule_crm management')) { ?>
    <fieldset class="edit_user_group">
    <?php 
      if (isset($user->profile_memberid)) {
          $cc_map_info = ncn_capsulecrm_get_map_info($user->profile_memberid);
          $organization_id = 0; $person_id = 0;
          if ($cc_map_info) {
            $organization_id = $cc_map_info['organization_id'];
            $person_id       = $cc_map_info['person_id'];
            $_url_cc_org = ncn_capsulecrm_get_url()."party/".$organization_id;
          }
      }
    ?>
    <?php if ( isset($user->profile_memberid) && is_member($user->profile_memberid) && ($GLOBALS['user']->uid==1 || user_access('ncn capsule_crm management')) ) { ?>
        <h2 class="sub-title">CapsuleCRM</h2>
        <form method="POST" id="update_org_party_id">
        <input type="hidden" name="tfunction" value="update_org_party_id">
        <table style="width:100%;">
            <tr>
                <td class="td-label">Organization Party ID:</td>
                <td><input type="text" name="org_id" value="<?php echo $organization_id; ?>" />&nbsp;&nbsp;<?php if (isset($_url_cc_org)) { print l($_url_cc_org, $_url_cc_org, array('attributes'=>array('target'=>'_blank'))); } ?></td>
            </tr>
            <tr>
                <td class="td-label">Person Party ID:</td>
                <td><input type="text" name="person_id" value="<?php echo $person_id; ?>" /></td>
            </tr>
            <tr>
                <td colspan='2'><input type="submit" value="Update" /></td>
            </tr>
        </table>
        </form>
    <?php } ?>
    <h2 class="sub-title">CapsuleCRM Account</h2>
    <form method="POST" id="update_capsulecrm_account_form">
    <input type="hidden" name="tfunction" value="update_capsulecrm_account">
    <table style="width:100%;">
      <tr>
        <td class="td-label">Account:</td>
        <td><?php print ncn_capsulecrm_account_select_render("casulecrm_account", ncn_capsulecrm_account_get_account($user->uid)); ?></td>
      </tr>
      <tr>
        <td colspan='2'><input type="submit" value="Set" /></td>
      </tr>
    </table>
    </form>
    
    </fieldset>
    <?php } ?>
    
    <?php 
    if ( ($GLOBALS['user']->uid==1 || user_access('ncn_admin edit member type')) && (isset($user->profile_memberid)&&is_member($user->profile_memberid))) {
        $member_type = get_member_type($user);
    ?>
    <fieldset class="edit_user_group">
        <h2 class="sub-title">Member Type</h2>
        <form method="POST" id="update_member_type">
        <input type="hidden" name="tfunction" value="update_member_type">
        <table style="width:100%;">
            <tr>
                <td class="td-label">Member Type:</td>
                <td>
                    <select id="member_type" name="member_type">
                        <option value="0" <?php if($member_type == 0){ echo "selected"; } ?> >NCN Gold Member</option>
                        <option value="1" <?php if($member_type == 1){ echo "selected"; } ?> >NCN Silver Member</option>
                        <option value="2" <?php if($member_type == 2){ echo "selected"; } ?> >NCN Gold Lite Member</option>
                        <option value="3" <?php if($member_type == 3){ echo "selected"; } ?> >NCN Coach on Call Member</option>
                        <option value="4" <?php if($member_type == 4){ echo "selected"; } ?> >NCN Gold Coach Member</option>
                        <option value="5" <?php if($member_type == 5){ echo "selected"; } ?> >CSI Member</option>
                    </select>
                </td>
            </tr>
            <tr>
                <td colspan='2'><input type="submit" value="Update Member Type" /></td>
            </tr>
        </table>
        </form>
    </fieldset>
    
  <fieldset class="edit_user_group">
    <h2 class="sub-title">Cancel/Pause Member</h2>
    <?php print ncn_user_show_block_track_table($user->uid); ?>
    <!--
    <?php if ($user->status): ?>
    <form method="POST" id="cancel_pause_member">
    <input type="hidden" name="tfunction" value="cancel_pause_member">
      <div>
        <label for="returned_datepicker">Returned Date: </label><input type="text" id="returned_datepicker" name="returned_datepicker" size="20" readonly>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        <input type="submit" value="Pause Member" onclick="return confirm('Are you sure you want to block member?');" />
        <script>
          $(function() {
            $( '#returned_datepicker' ).datepicker({
              showOn: "button",
              buttonImage: "<?php global $base_url; print $base_url.drupal_get_path('module', 'ncn_report')."/images/calendar.gif"; ?>",
              buttonImageOnly: true,
              dateFormat: 'yy/mm/dd',
            });
          });
        </script>
      </div>
      <div class="description">If you don't specify returned date, it would cancel member.</div>
    </form>
    <?php else: ?>
    <form method="POST" id="unblock_member">
      <input type="hidden" name="tfunction" value="unblock_member">
      <input type="submit" value="Unblock Member" onclick="return confirm('Are you sure you want to unblock member?');" />
    </form>
    <?php endif; ?>
    -->
  </fieldset>

  <fieldset class="edit_user_group">
    <h2 class="sub-title">Member Renewal Date</h2>
    <form method="POST" id="change_member_renewal_date">
    <input type="hidden" name="tfunction" value="change_member_renewal_date">
    <div style="margin-top: 20px;">
      <?php print render_ncn_admin_member_renewal_select("ncn_admin_member_renewal_date", $user->profile_memberid); ?>&nbsp;&nbsp;<label>on Every Month</label>
      &nbsp;&nbsp;&nbsp;&nbsp;
      <input type="submit" value="Change" />
    </div>
    </form>
  </fieldset>
    
    <fieldset class="edit_user_group">
        <h2 class="sub-title">Send Claim Reminder Mail</h2>
        <form method="POST" id="send_claim_reminder_mail">
        <input type="hidden" name="tfunction" value="send_claim_reminder_mail">
        <table style="width:100%;">
            <tr>
                <td>
                    <input type="submit" value="Send Claim Reminder Mail" />
                </td>
            </tr>
        </table>
        </form>
    </fieldset>
    
    <?php $grant_subuser = ncn_subuser_has_grant_to_create_subuser(isset($user->profile_memberid)?$user->profile_memberid:0); ?>
    <fieldset class="edit_user_group">
        <h2 class="sub-title">Grant to create sub user</h2>
        <form method="POST" id="grant_subuser_form">
        <input type="hidden" name="tfunction" value="grant_subuser">
        <table style="width:100%;">
            <tr>
                <td class="td-label">Give a grant to create sub user:</td>
                <td>
                    <select id="grant_subuser" name="grant_subuser">
                        <option value="0" <?php if($grant_subuser == FALSE){ echo "selected"; } ?> >No</option>
                        <option value="1" <?php if($grant_subuser == TRUE){ echo "selected"; } ?> >Yes</option>
                    </select>
                </td>
            </tr>
            <tr>
                <td colspan='2'><input type="submit" value="Save" /></td>
            </tr>
        </table>
        </form>
    </fieldset>
    <?php } ?>
    
    
    <?php 
    if ($in_admin && isset($user->profile_memberid) && is_member($user->profile_memberid)) { 
        $owner_id = get_owner($user->profile_memberid);
        if ($owner_id != 0) {
    ?>
    <fieldset class="edit_user_group">
        <h2 class="sub-title">Distributor/Associate</h2>
        
        <form method="POST" id="update_dist_asso_form">
        <input type="hidden" name="tfunction" value="update_dist_asso">
        <table style="width:100%;">
            <tr class="distributor-list">
                <td class="td-label">Type:</td>
                <td>
                    <select id="dist_asso" name="dist_asso">
                        <option value="distributor" <?php if(is_distributor(user_load($owner_id))){ echo "selected"; } ?> >Distributor</option>
                        <option value="associate"   <?php if(is_associate  (user_load($owner_id))){ echo "selected"; } ?> >Associate</option>
                    </select>
                </td>
            </tr>
            <tr class="distributor-list da-dropdown">
                <td class="td-label">Distributor:</td>
                <td><?php distributor_dropdown($owner_id); ?></td>
            </tr>
            <tr class="associate-list da-dropdown">
                <td class="td-label">Associate</td>
                <td><?php associate_dropdown($owner_id); ?></td>
            </tr>
            <tr>
                <td colspan='2'><input type="submit" value="Update Distributor/Associate" /></td>
            </tr>
        </table>
        </form>
    </fieldset>
        
    <?php
        } else {
            drupal_set_message("Owner of this user isn't setted", "error");
        }
    }
    
    // Update Account Manager
    if ($in_admin && isset($user->profile_memberid) && is_member($user->profile_memberid)) { 
        $am_uid = 0;
    //  $query = "SELECT * FROM member_id_pool WHERE member_id=".$user->profile_memberid;
        $result = db_query('SELECT * FROM {member_id_pool} WHERE member_id=:a',array(':a'=>$user->profile_memberid));
        if ($result->rowCount()>0) {
            $row = $result->fetchAssoc();
        //  $row = (array)$result;
            $am_uid = $row['am_uid'];
        }
    ?>
    <fieldset class="edit_user_group">
        <h2 class="sub-title">Account Manager</h2>
        
        <form method="POST" id="update_account_manager_form">
        <input type="hidden" name="tfunction" value="update_account_manager">
        <table style="width:100%;">
            <tr class="am-list">
                <td class="td-label">Account Manager:</td>
                <td>
                    <?php echo draw_select_account_manager_list('am_uid',  $am_uid); ?>
                </td>
            </tr>
            <tr>
                <td colspan='2'><input type="submit" value="Update" /></td>
            </tr>
        </table>
        </form>
    </fieldset>
        
<?php
    }
        
            
    if ((is_distributor($user) == true) && stristr($_SERVER['REQUEST_URI'], 'admin')) {
                // show credit card form
?>
        <fieldset class="edit_user_group">
        <h2 class="sub-title">Report Password</h2>
        <!-- <legend>Credit Card Details</legend> -->
        <form method="POST" id="change_report_pass">
            <input type="hidden" name="tfunction" value="change_report_pass" />
            <input type="hidden" name="username" value="<?= $user->name; ?>">
            <table style="width:100%;border:0;">
                <tr>
                    <td class="td-label">Old Password:</td>
                    <td><input type="password" name="old_pass" ></td>
                </tr>
                <tr>
                    <td class="td-label">New Password:</td>
                    <td><input type="password" name="new_pass" ></td>
                </tr>
                <tr>
                    <td class="td-label">Confirm Password:</td>
                    <td><input type="password" name="confirm_pass" ></td>
                </tr>
                <tr>
                    <td class="td-label">&nbsp;</td>
                    <td><input type="submit" value="Update" ></td>
                </tr>
            </table>
        </form>
        </fieldset>
<?php 
    }
    
    // only show credit card and document upload for non account managers
    
    if ($in_admin && (is_distributor($user) || (isset($user->profile_memberid) &&is_member($user->profile_memberid))))
    {
        // show credit card form
        ?>
        <fieldset class="edit_user_group">
        <h2 class="sub-title">Credit Card Details</h2>
        <!-- <legend>Credit Card Details</legend> -->
        
        <form method="POST" id="update_credit_card_form">
        <input type="hidden" name="tfunction" value="update_cc">
        <table style="width:100%;">
            <!-- <tr>
                <td class="td-label">Credit Card Number:</td>
                <td><input type="text" name="credit_card_number" style="width:200px;"></td>
            </tr>
            <tr>
                <td class="td-label">Expiration Date (mm/yy):</td>
                <td><input type="text" name="credit_card_expiration" style="width:200px;"></td>
            </tr> -->
            <tr>
                <td class="td-label">Security Code:</td>
                <td><input type="text" name="credit_card_security" value="<?php echo $user->profile_credit_card_security; ?>" style="width:200px;"></td>
            </tr>
            <tr>
                <td class="td-label"></td>
                <td><a class="update-credit-card-btn" href="#" onclick="update_credit_card_submit();">Update</a></td>
            </tr>
        </table>
        </form>
        </fieldset>
        <?php
        
        // show edit practicepay profile id#'s form for these users (but only in admin)
        if (stristr($_SERVER['REQUEST_URI'], 'admin') && (is_distributor($user) || (isset($user->profile_memberid)&&is_member($user->profile_memberid))))
        {
            // get the current data
            $cim_profile = get_cim_profile_data($user); // see function below
            
            // show the form part
            ?>
            <fieldset class="edit_user_group">
            <h2 class="sub-title">Practice Pay Solutions Profile Data</h2>
            <!-- <legend>Practice Pay Solutions Profile Data</legend> -->
            
            <form method="POST">
            <input type="hidden" name="tfunction" value="update_practicepay_profile">
                <table style="width:100%;">
                    <tr>
                        <td class="td-label">customerProfileId</td>
                        <td><input type="text" name="customerProfileId" value="<?= $cim_profile['customerProfileId']; ?>" style="width:200px;" />
                    </tr>
                    <tr>
                        <td class="td-label">customerPaymentProfileId</td>
                        <td><input type="text" name="customerPaymentProfileId" value="<?= $cim_profile['customerPaymentProfileId']; ?>" style="width:200px;" />
                    </tr>
                    <tr>
                        <td class="td-label"></td>
                        <td><input type="submit" value="Update PracticePay Profile"></td>
                    </tr>
                </table>
            </fieldset>
            <?php
        }
                
    }

    // document center additions    
    if (false) {
    if ((is_account_manager($user) == false) && (is_distributor($user) == false) && (stristr($_SERVER['REQUEST_URI'], 'admin')))
    {
        // document upload and list
        ?>
        <fieldset class="edit_user_group">
        <h2 class="sub-title">User Documents</h2>
        <!--<legend>User Documents</legend>-->
        
            <?php
            print drupal_get_form('ncn_admin_upload_document'); 
            
            // see if there are any documents
        //  $query = "SELECT * FROM member_documents WHERE uid=".$user->uid." ORDER BY title ASC;";
            $result = db_query('SELECT * FROM {member_documents} WHERE uid=:a ORDER BY title ASC',
            array(':a'=>$user->uid));
            $row_count = $result->rowCount();
            
            if ($row_count > 0)
            {
                ?>
                <div style="width:500px;">
                <table class="sticky-enabled tableSelect-processed sticky-table page_table" id="page_table_1">
                    <tbody>
                <?php                           
            
                for ($i=0; $i<$row_count; $i++)
                {
                    $row = (array)$result;
                    
                    ?>
                    <tr <?php if ($i % 2) { print 'class="odd"'; } else { print 'class="even"'; }?> >
                        <td><a href="/account/serve_user_document/<?= $row['id']; ?>/<?= $row['uid']; ?>"><?= StripSlashes($row['title']); ?></a></td>
                        <td><a class='delete_link' href="/admin/delete_user_document/<?= $row['id']; ?>/<?= $row['uid']; ?>" onclick="return confirm('Are you sure you want to delete this document?');">[delete]</td>
                    </tr>
                    <?php
                    
                }
                
                ?>
                    </tbody>
                    </table>    
                    </div>          
                <?php
            }
            // no documents found
            else
            {
                ?>No documents have been uploaded for this user.<?php
            }
        
            ?>
        
        </fieldset>
        <?php
    }}
}
