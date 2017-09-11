<?php
function get_claim_workflow_array() {
    return array(
        'none'                          => 'None', 
        'fax_doc_recieved'  => 'Fax Documents Received',
        'all_doc_recieved'  => 'All Documents Received',
        'doc_clarification' => 'Document Clarification',
        'rejected_invoices' => 'Rejected Invoices',
        'rejected_claim_docs'=>'Rejected Claim Documents',
        'completed_status'  => 'Completed Claim Status'
    );
}

//------------------------------------------------------------------------------
// view individual claim
function ncn_admin_view_claim($claim_id)
{
    GLOBAL $base_url;
  
    ncn_admin_view_claim_action($claim_id);

    if (isset($_POST['current_scroll_position']) && $_POST['current_scroll_position'] != "")
    {
        echo "<script language='javascript'> window.onload = function(){window.scrollTo(0, ". $_POST['current_scroll_position'] .");} </script>";
    }
    else if (isset($_GET['current_scroll_position']) &&  $_GET['current_scroll_position'] != "")
    {
        echo "<script language='javascript'> window.onload = function(){window.scrollTo(0, ". $_GET['current_scroll_position'] .");}</script>";
    }
    else if (isset($_SESSION['current_scroll_position']))
    {
        echo "<script language='javascript'> window.onload = function(){window.scrollTo(0, ". $_SESSION['current_scroll_position'] .");}</script>";
        $_SESSION['current_scroll_position'] = 0;
    }

        
    if (isset($_POST['tfunction_workflow']) && $_POST['tfunction_workflow'] == 'tfunction_workflow') {
        ncn_admin_claim_workflow_change($claim_id);
        // mark 1st claim mail reminder
        ncn_admin_mark_to_send_1st_mail($claim_id);     
    } else if (isset($_POST['tfunction_file_note']) && $_POST['tfunction_file_note'] == 'tfunction_file_note') {
        ncn_admin_claim_file_note_change($claim_id);
    } else if (isset($_POST['tfunction_claim_type']) && $_POST['tfunction_claim_type'] == 'claim_type_change') {
        ncn_admin_claim_type_change($claim_id);
    } else if (isset($_POST['tfunction_claim_note_action']) && $_POST['tfunction_claim_note_action'] == 'tfunction_claim_note_add') {
        $_claim_note = trim($_REQUEST['claim_note']);
        if ($_claim_note) {
          ncn_admin_note_add_note_from_original($claim_id);
          if (ncn_admin_note_add_note($claim_id, $GLOBALS['user']->uid, $_claim_note)) {
            drupal_set_message("You added note to claim, successfully.");
            $log_message = t("!user_name added Claim Note. !note", array('!user_name'=>ncn_amin_get_user_role_and_name(), '!note'=>strip_tags($_claim_note)));
            ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
          }
          /*if (ncn_admin_add_claim_note($claim_id, $_claim_note)) {
            drupal_set_message("You added note to claim, successfully.");
            $log_message = t("!user_name added Claim Note. !note", array('!user_name'=>ncn_amin_get_user_role_and_name(), '!note'=>strip_tags($_claim_note)));
            ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
          }*/
        } 
    } else if (isset($_POST['tfunction_claim_note_action']) && $_POST['tfunction_claim_note_action'] == 'tfunction_claim_note_update') {
    $note_id = $_REQUEST['claim_note_id'];
    $_claim_note = trim($_REQUEST['claim_note']);
    if ($_claim_note) {
      if (ncn_admin_note_update_note($note_id, $_claim_note)) {
        drupal_set_message("You updated note to claim, successfully.");
        $log_message = t("!user_name update Claim Note. !note", array('!user_name'=>ncn_amin_get_user_role_and_name(), '!note'=>strip_tags($_claim_note)));
        ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
      }
    }
  }
  
    if (isset($_POST['tfunction_send_cdr']) && $_POST['tfunction_send_cdr'] == 'tfunction_send_cdr') {
        ncn_admin_send_cdr($claim_id);
    }
    
    // --- handle of uploading scope file ---
    if (isset($_POST['scope_upload_function']) && $_POST['scope_upload_function'] == 'scope') {
        $roomname = $_POST['roomname'];
        ncn_admin_scopeform_submit($roomname);
    } else if (isset($_POST['scope_upload_function']) && $_POST['scope_upload_function'] == 'scope_auto_full_pdf') {
        $roomname = $_POST['roomname'];
        ncn_admin_auto_scopeform_full_pdf_submit(arg(4), $roomname);
    } else if (isset($_POST['scope_upload_function']) && $_POST['scope_upload_function'] == 'scope_ss_data') {
        $roomname = $_POST['roomname'];
        ncn_admin_ss_make_scopesheet_image(arg(4), $roomname);
    }
    
    if (isset($_POST['scope_upload_function']) && $_POST['claims_data_update'] == 'claims_data_update') {
        ncn_admin_claimsdata_update($claim_id);
    }
    
    if (isset($_POST['create_invoice_function']) && $_POST['create_invoice_function'] == 'create_invoice') {
        generate_final_invoice($claim_id);
    }
    
    if (isset($_POST['extend_data_function']) && $_POST['extend_data_function'] == 'extend_data') {
        save_extended_data($claim_id);
    }
    $invoice_data = get_extended_data($claim_id);
    
    if (isset($_POST['photo_add_room']) && $_POST['photo_add_room']  == 'photo_add_room') {
        ncn_admin_photo_add_room($claim_id, $_POST['room_name']);
    }
    else if (isset($_POST['update_captions']) && $_POST['update_captions'] == 'update_captions') {
        save_image_captions($claim_id);
    }
    else if (isset($_POST['photo_delete_room']) && $_POST['photo_delete_room']  == 'photo_delete_room') {
        ncn_admin_photo_delete_room($claim_id, $_POST['room_name']);
    } 
    else if (isset($_POST['photo_delete_first_room']) && $_POST['photo_delete_first_room']  == 'photo_delete_first_room') {
        ncn_admin_photo_delete_first_room($claim_id, $_POST['room_name']);
    } 
    
    if (stristr($_SERVER['REQUEST_URI'], 'ncn_admin_pool'))
    {   $url_part = "ncn_admin_pool";       }
    else
    {   $url_part = "ncn_admin";        }

    
    // --- handle status update ----
    if ( isset($_POST['claim_status_change']) && $_POST['claim_status_change']=='claim_status_change' && isset($_POST['claim_status']) )
    {
        // update claim amount
        if ((is_numeric(trim($_POST['claim_amount']))) && (trim($_POST['claim_amount'])>= 0))
        {
        //  $query = "UPDATE claims SET claim_amount=".$_POST['claim_amount']." WHERE claim_id=".$claim_id;
            $result = db_query('UPDATE {claims} SET claim_amount=:a  WHERE claim_id=:b',
            array(':a'=>$_POST['claim_amount'],':b'=>$claim_id));
        }
        else    // invalid (non numeric) value entered for "claim amount"
        {
            drupal_set_message('Claim amount must be a positive number.' , 'error');
        }
    
        // update payment received amount
        if ((is_numeric(trim($_POST['payment_received']))) && (trim($_POST['payment_received'])>= 0))
        {
        //  $query = "UPDATE claims SET payment_received=".trim($_POST['payment_received'])." WHERE claim_id=".$claim_id;
            $result = db_query('UPDATE {claims} SET payment_received=:a WHERE claim_id=:b',
            array(':a'=>trim($_POST['payment_received']),':b'=>$claim_id));
        }
        else    // invalid (non numeric) value entered for "payment_received"
        {
            drupal_set_message('Payment received amount must be a positive number.' , 'error');
        }
    
        // carry on
        
        
        $updated_user = $GLOBALS['user']->name;
        $log_message = "Status updated by " . ncn_amin_get_user_role_and_name() . " to '".$_POST['claim_status']."' ";
        
        if (trim($_POST['claim_status_message']) != '') // only add status message to log if it's not blank
        {
            $log_message .= "(with message: ".$_POST['claim_status_message'].")";
        }
        
        $log_message .= " Claim amount updated to: $".trim($_POST['claim_amount']);
        drupal_set_message($log_message, 'status');
        
        ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
        
        // --- email approved ---
        if ((isset($_POST['claim_status']) && $_POST['claim_status'] == 'approved') && (variable_get('ncn_approved_active', '') == 1))
        {
        //  $query = "SELECT user_id FROM claims WHERE claim_id=".$claim_id;
            $result = db_query('SELECT user_id FROM {claims} WHERE claim_id=:a',array(':a'=>$claim_id));
            $row = $result->fetchAssoc();
        
            $subject = variable_get('ncn_approved_subject', '');
            $body = variable_get('ncn_approved_body', '');
        
            $user_details = user_load($row['user_id']); 
            $v['claim_id'] = $claim_id;
            $v['customer_name'] = ncn_cd($claim_id, 'customer_name');
        
            $subject = pm_message_tags($v, $subject);
            $body = pm_message_tags($v, $body);
            $params = array(
                '!owner_name' => ncn_cd($claim_id, 'customer_name'),
                '!claim_id' => $claim_id
            );
            
            pm_send(0, $user_details->uid, $subject, $body, $claim_id, 0, 'approve_claim', $params);
        }
        else
        // --- email rejected ---
        if ((isset($_POST['claim_status']) && $_POST['claim_status'] == 'returned') && (variable_get('ncn_approved_active', '') == 1))
        {
        //  $query = "SELECT user_id FROM claims WHERE claim_id=".$claim_id;
            $result = db_query('SELECT user_id FROM {claims} WHERE claim_id=:a',array(':a'=>$claim_id));
            $row = $result->fetchAssoc();
        
            $subject = variable_get('ncn_rejected_subject', '');
            $body = variable_get('ncn_rejected_body', '');
        
            $user_details = user_load($row['user_id']); 
            $v['claim_id'] = $claim_id;
            $v['customer_name'] = ncn_cd($claim_id, 'customer_name');
            $v['reason'] = $_POST['claim_status_message'];
        
            $subject = pm_message_tags($v, $subject);
            $body = pm_message_tags($v, $body);
            
            pm_send(0, $user_details->uid, $subject, $body, $claim_id);
        }
        // --- claim message changed? ----
        else
        if (isset($_POST['claim_status_message']) && trim($_POST['claim_status_message']) != '')
        {
            // check to see if claims message has changed
        //  $query = "SELECT * FROM claims WHERE claim_id=".$claim_id;
            $result = db_query('SELECT * FROM {claims} WHERE claim_id=:a',array(':a'=>$claim_id));
            $row = $result->fetchAssoc();
            
            if (AddSlashes($_POST['claim_status_message']) != $row['claim_status_message'])
            {
                $subject = variable_get('ncn_generic_subject', '');
                $body = variable_get('ncn_generic_body', '');
            
                $user_details = user_load($row['user_id']); 

                $v['claim_id'] = $claim_id;
                $v['customer_name'] = ncn_cd($claim_id, 'customer_name');
                $v['status'] = $_POST['claim_status_message'];
            
                $subject = pm_message_tags($v, $subject);   
                $body = pm_message_tags($v, $body);
                
                pm_send(0, $user_details->uid, $subject, $body, $claim_id);
                
                
            } 
        }
        
        // update claim status
    //  $query = "UPDATE claims SET claim_status='".$_POST['claim_status']."',claim_status_message=\"".AddSlashes($_POST['claim_status_message'])."\",last_modified=".date('U')." WHERE claim_id=".$claim_id;
        $result = db_query('UPDATE {claims} SET claim_status=:a, claim_status_message=:b,
        last_modified=:c  WHERE claim_id=:d',array(':a'=>$_POST['claim_status'],
        ':b'=>$_POST['claim_status_message'],':c'=>date('U'),':d'=>$claim_id));
        
        // Claim Back End
        set_ncn_data_extra($claim_id, 'claim_backend', $_POST['claim_backend']);
    }
    
    // ---- display stuff start ----
    // get logfile count
//  $query2 = "SELECT * FROM claims_log WHERE claim_id=".$claim_id.";";
    $result2 = db_query('SELECT * FROM {claims_log} WHERE claim_id=:a',array(':a'=>$claim_id));
    $entries_in_log = $result2->rowCount();
    
    // get claim status'
    $query = "SHOW COLUMNS FROM `claims` LIKE 'claim_status'";
    $result = db_query($query);
    $row = $result->fetchAssoc();
    
    $statuses = str_replace("'", "", $row['Type']);
    $statuses = str_replace("enum(", "", $statuses);
    $statuses = str_replace(")", "", $statuses);
    $statuses = explode(',', $statuses);
    
        

    // ---- draw controls -----
//  $query = "SELECT * FROM claims WHERE claim_id=".$claim_id.";";
    $result = db_query('SELECT * FROM {claims} WHERE claim_id=:a',array(':a'=>$claim_id));
    $row_count = $result->rowCount();
    if ($row_count == 0) {
        drupal_set_message(t('Claim(#!claim_id) doesn\'t exist.', array('!claim_id'=>$claim_id)), 'error');
        drupal_goto('admin/config/ncn_admin');
    }
    $row = $result->fetchAssoc();
    
    ?>
    
    <?php 
        /*************** Permission of View Claim ***************/
        //* Claim Overview 
        $pvc_claim_status       = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Status');  
        $pvc_claim_type         = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Type');    
        
        $pvc_claim_workflow     = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Workflow'); 
        $pvc_claim_note         = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Note'); 
        $pvc_send_cdr   = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Send Claim Documents Reminder'); 

        $pvc_claim_status_disabled = '';
        if (!($pvc_claim_status&2))     {$pvc_claim_status_disabled="disabled";}
        $pvc_claim_type_disabled = '';
        if (!($pvc_claim_type&2))       {$pvc_claim_type_disabled="disabled";}
        $pvc_claim_workflow_disabled = '';
        if (!($pvc_claim_workflow&2))       {$pvc_claim_workflow_disabled="disabled";}
        $pvc_claim_note_disabled = '';
        if (!($pvc_claim_note&2))           {$pvc_claim_note_disabled="disabled";}
        $pvc_send_cdr_disabled = '';
        if (!($pvc_send_cdr&2))             {$pvc_send_cdr_disabled="disabled";}
        
        
        //* Claim View - CE Portal Admin Panel
        $pvc_ce_management  = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Exaimners Management'); 
        $pvc_ce_management_disabled = '';
        if (!($pvc_ce_management&2))        {$pvc_ce_management_disabled="disabled";}
        
        //* Upload Claim Documents
        $pvc_upload_claim_doc   = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Upload Claim Documents'); 
        $pvc_upload_claim_doc_disabled = '';
        if (!($pvc_upload_claim_doc&2))     {$pvc_upload_claim_doc_disabled="disabled";}
        //* Upload CE Documents
        $pvc_upload_ce_doc  = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Upload CE Documents');
        $pvc_upload_ce_doc_disabled = '';
        if (!($pvc_upload_ce_doc&2))        {$pvc_upload_ce_doc_disabled="disabled";}
        //* Upload ESX Files
        $pvc_upload_esx_file    = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Upload ESX Files'); 
        $pvc_upload_esx_file_disabled = '';
        if (!($pvc_upload_esx_file&2))      {$pvc_upload_esx_file_disabled="disabled";}
        //* Upload CE Invoice PDF Files
        $pvc_upload_ce_invoice_file     = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Upload CE Invoice PDF Files'); 
        $pvc_upload_ce_invoice_file_disabled = '';
        if (!($pvc_upload_ce_invoice_file&2))       {$pvc_upload_ce_invoice_file_disabled="disabled";}
        
        //* Claim Data
        $pvc_claim_data         = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Data'); 
        $pvc_claim_invoice_data = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Invoice Data'); 
        $pvc_claim_invoice_doc  = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Claim Invoice Documents'); 
        $pvc_claim_data_disabled = '';
        if (!($pvc_claim_data&2))           {$pvc_claim_data_disabled="disabled";}
        $pvc_claim_invoice_data_disabled = '';
        if (!($pvc_claim_invoice_data&2))   {$pvc_claim_invoice_data_disabled="disabled";}
        $pvc_claim_invoice_doc_disabled = '';
        if (!($pvc_claim_invoice_doc&2))    {$pvc_claim_invoice_doc_disabled="disabled";}
        
        //* Photo Album
        $pvc_photo_album        = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Photo Album'); 
        $pvc_photo_album_disabled = '';
        if (!($pvc_photo_album&2))          {$pvc_photo_album_disabled="disabled";}
        
        //* Create Invoice
        $pvc_create_invoice     = ncn_pvc_get_permission_from_user($GLOBALS['user']->uid, 'Create Invoice');
        $pvc_create_invoice_disabled = ''; 
        if (!($pvc_create_invoice&2))       {$pvc_create_invoice_disabled="disabled";}
        
    ?>
    
    <div>
        <fieldset>
        <legend>Claim Overview</legend>
        <form method="POST">
        <input type="hidden" name="claim_status_change" id="claim_status_change" value="" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
        <table> 
            <thead></thead>
            <tbody style="border:0;">
            <tr valign="top">
            <td style="padding:0;">
                <div>
                <table>
                    <thead></thead>
                    <tbody style="border:0;">
                        <?php if ($pvc_claim_status&01): ?>
                        <tr valign="top">           
                            <td><strong>Claim Type</strong></td>
                            <td>
                                <?php if ($pvc_claim_type_disabled=='disabled'): ?>
                                    <?php echo ucwords($row['claim_type']); ?> &nbsp;&nbsp;
                                    <?php echo ucwords($row['claim_product']); ?>&nbsp;&nbsp;
                                <?php else: ?>
                                    <input type="hidden" name="tfunction_claim_type" id="tfunction_claim_type" value="" />
                                    <?php echo ncn_admin_draw_select_claim_type('claim_type', $row['claim_type']); ?>   <br />
                                    <?php echo ncn_admin_draw_select_claim_product('claim_product', $row['claim_product']); ?>&nbsp;&nbsp;
                                    <input type="submit" value="Change" onclick="return on_click_change_claim_type();" />
                                    
                                <?php endif; ?>
                            </td>
                        </tr>
                        <tr>            
                            <td><strong>Date Created</strong></td>
                            <td><?= date("m/d/Y", $row['created']); ?></td>
                        </tr>
                        <tr>            
                            <td><strong>Last Modified</strong></td>
                            <td><?= date("m/d/Y", $row['last_modified']); ?></td>
                        </tr>
            
                        <?php if (user_access('ncn_admin edit addtional claim data')): ?>
                        <tr>
                            <td><strong>Claims Processing</strong></td>
                            <td>
                <a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/view-additional-claim-data/<?= $claim_id; ?>" target='_blank'>View</a> / 
                <a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/edit-additional-claim-data/<?= $claim_id; ?>" target='_blank'>Edit<!--Additional Data--></a>
                Claims Processing
              </td>
                        </tr>
            <?php elseif (user_access('ncn_admin view addtional claim data')): ?>
            <tr>
              <td><strong>Claims Processing</strong></td>
              <td>
                <a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/view-additional-claim-data/<?= $claim_id; ?>" target='_blank'>View</a> 
                Claims Processing
              </td>
            </tr>
                        <?php endif; ?>
            
                        <tr>
                            <td><strong>Log file</strong></td>
                            <td><a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/viewlog/<?= $claim_id; ?>"><?= $entries_in_log;?> entries in log</a></td>
                        </tr>
                        <tr>
                            <td><strong>Claim Amount</strong></td>
                            <td>$<input type="text" name="claim_amount" value="<?= $row['claim_amount']; ?>" style="width:120px;" /></td>
                        </tr>
                        <tr>
                            <td><strong>Payment Received</strong></td>
                            <td>$<input type="text" name="payment_received" value="<?= $row['payment_received']; ?>" style="width:120px;" /></td>
                        </tr>
                        <tr valign="top">
                            <td><strong>File Notes</strong></td>
                            <td>
                                <input type="hidden" name="tfunction_file_note" id="tfunction_file_note" value="" />
                                <textarea name="claim_file_note" style="height:70px; min-width: 200px;" ></textarea>
                                <?php if ($pvc_claim_status_disabled!='disabled'): ?>
                                <div><input type="submit" value=" Set " onclick="return on_click_change_file_note();" /></div>
                                <?php endif; ?>
                            </td>
                        </tr>
                        <?php endif; ?>
                    </tbody>
                </table>
                </div>
            </td>
            <td style="padding:0;">
                <div>
                <table>
                    <thead></thead>
                    <tbody style="border:0;">
                        <?php if ($pvc_claim_status&01): ?>
                        <tr>
                            <td><strong>Download Excel file</strong></td>
                            <td><a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/downloadxls/<?= $claim_id; ?>">claim-data_<?= $claim_id; ?>.xls</a></td>
                        </tr>
                        <tr>
                            <td><strong>Download PDF file</strong></td>
                            <td><a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/downloadpdf/<?= $claim_id; ?>">claim-data_<?= $claim_id; ?>.pdf</a></td>
                        </tr>
                        <tr>            
                            <td><strong>Claim Status</strong></td>
                            <td>
                                <select name="claim_status" >
                                    <?php
                                    
                                    foreach ($statuses as $status)
                                    {
                                        if ($status == $row['claim_status'])
                                        {   $checked = " selected ";    }
                                        else
                                        {   $checked = '';  }
                                    
                                        ?>
                                        <option value="<?= $status; ?>" <?= $checked; ?> ><?= ucwords($status); ?></option>
                                        <?php
                                    }
                                    
                                    ?>
                                </select>
                            </td>
                        </tr>
                        <tr>            
                            <td valign="top"><strong>Claim Status Message</strong></td>
                            <td>
                                <textarea name="claim_status_message" style="height:70px;" ></textarea>
                            </td>
                        </tr>
                        <tr>
                            <td><strong>Claim Back End</strong></td>
                            <td><?php echo draw_select_claim_view_claim_backend('claim_backend', get_ncn_data_extra($claim_id, 'claim_backend') ); ?></td>
                        </tr>
                        
                        <tr>
                            <td></td>
                            <td>
                                <?php if ($pvc_claim_status_disabled!='disabled'): ?>
                                <input type="submit" value="Update Claim Status" onclick="return on_click_change_claim_status();" >
                                <?php endif; ?>
                            </td>
                        </tr>
                        <?php endif;    // Claim Status?>
                    </tbody>            
                </table>
                </div>
            </td>
            </tr>
            </tbody>
        </table>    
        </form>
        
        <?php if ($pvc_claim_note&01): ?>
        <form id="ncn_admin_claim_note_form" method="POST" class="claim-note-add-form">
        <input type="hidden" name="tfunction_claim_note_action" id="tfunction_claim_note_action" value="tfunction_claim_note_add" />
    <input type="hidden" name="claim_note_id" id="claim_note_id" value="0" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
        <table>
            <tr>
                <td valign="top" style="width:50px"><strong>Note</strong></td>
                <td>
                    <?php 
            if (ncn_admin_note_check_mode($claim_id) == 'new') { print ncn_admin_note_render_claim_notes_table($claim_id, $GLOBALS['user']->uid); }
            else { ?> 
            <textarea rows="5" cols="80" readonly><?php $_claim_note = str_replace("<br/>", "\n", base64_decode($row['file_note']));  echo $_claim_note; ?></textarea> 
          <?php } ?>
                </td>
            </tr>
            <?php if ($pvc_claim_note_disabled!='disabled'): ?>
            <tr>
                <td></td>
                <td><input id="ncn_admin_claim_note_content" type="text" name="claim_note" size="80" /></td>
            </tr>
            <tr>
                <td></td>
                <td><input id="ncn_admin_claim_note_submit_btn" type="submit"  onclick="return on_set_scroll_position();" value="Add" />&nbsp;&nbsp;&nbsp;
            <input id="ncn_admin_claim_note_cancel_btn" type="button"  onclick="on_click_claim_note_cancel_btn(); "   value="Cancel" />
        </td>
            </tr>
            <?php endif; ?>
        </table>
        </form>
        <?php endif; ?>
        
        <?php if ($pvc_claim_workflow&01): ?>
        <form id="ncn_admin_claim_workflow_form" method="POST">
        <input type="hidden" name="tfunction_workflow" id="tfunction_workflow" value="tfunction_workflow" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
        <table>
            <tr>
                <td valign="top" style="width:180px"><strong>Claim Workflow</strong></td>
                <td>                    
                    <select id="claim_workflow" name="claim_workflow">
                    <?php
                    $arr_timer = get_claim_workflow_array();
                    foreach ($arr_timer as $key=>$val)
                    {
                        if ($key == $row['workflow'])
                        {   $checked = " selected ";    }
                        else
                        {   $checked = '';  }
                    
                        ?>
                        <option value="<?= $key; ?>" <?= $checked; ?>><?= $val; ?></option>
                        <?php
                    }
                    
                    ?>
                    </select>
                    <?php if ($pvc_claim_workflow_disabled!='disabled'): ?>
                    <input type="submit" onclick="return on_set_scroll_position();" value="Change"/>
                    <?php endif; ?>
                    
                    <div id="claim_timer_div">
                        <?php echo render_claim_timer($claim_id); ?>
                    </div>
                    
                    <?php if ($pvc_claim_workflow_disabled!='disabled'): ?>
                    <?php 
                        //$claim_docs_rejected_msg = variable_get('ncn_rejected_claim_docs_mail_body', '');
                        //$claim_docs_rejected_msg = ncn_admin_get_claim_docs_rejected_message($claim_id);
                    ?>
                    <div id="ncn_admin_rejected_claim_docs_mail_body" <?php if ($row['workflow']!='rejected_claim_docs') {echo 'style="display: none;"';} ?>>
                        <textarea name="rejected_claim_docs_mail_body" style="height:120px; min-width: 500px;"><?php echo $claim_docs_rejected_msg; ?></textarea>
                    </div>
                    <script>
                        jQuery(document).ready(function() {
                            jQuery('#claim_workflow').bind('change', function() {
                                if( jQuery('#claim_workflow').val()=='rejected_claim_docs') {
                                    jQuery('#ncn_admin_rejected_claim_docs_mail_body').css('display', 'block');
                                } else {
                                    jQuery('#ncn_admin_rejected_claim_docs_mail_body').css('display', 'none');
                                }
                            });
                        });
                    </script>
                    <?php endif; ?>
                </td>
            </tr>
        </table>
        </form>
        <?php endif; // Claim Workflow?>
        
        
        <!--- Send Claim Documents Reminder -->
        <?php if ($pvc_send_cdr_disabled!='disabled'): ?>
        <form id="ncn_admin_send_claim_documents_reminder" method="POST">
            <input type="hidden" name="tfunction_send_cdr" value="tfunction_send_cdr" />
            <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
            <input type="submit" onclick="return on_set_scroll_position();" value="Send Claim Documents Reminder" />
            <p style="font-style: italic; font-size: 11px;">Send a notification mail to said that we have not received claim documents.</p>
        </form>
        <?php endif; ?>
        <!--- END of Send Claim Documents Reminder -->
        
        <?php echo ncn_admin_render_claim_status($claim_id); ?>
        </fieldset>
    
    </div>
    <script type="text/javascript">
        function on_click_change_file_note() {
            jQuery('#tfunction_file_note').val('tfunction_file_note');
            jQuery('.current_scroll_position').val(get_current_scroll_position());
            return true;
        }
        function on_click_change_claim_status() {
            jQuery('#claim_status_change').val('claim_status_change');
            jQuery('.current_scroll_position').val(get_current_scroll_position());
            return true;
        }
        function on_click_change_claim_type() {
            jQuery('#tfunction_claim_type').val('claim_type_change');
            jQuery('.current_scroll_position').val(get_current_scroll_position());
            return true;
        }
        function on_set_scroll_position() {
            jQuery('.current_scroll_position').val(get_current_scroll_position());
            return true;
        }
    
        
    </script>
    <div id="ce_manager_wrapper">
        <?php print ncn_ceportal_claimview_claim_exaimners($claim_id, $pvc_ce_management, $pvc_ce_management_disabled); ?>
    </div>
    <div>
    <?php
    
    if ($pvc_upload_claim_doc&01) {
        echo ncn_admin_draw_invoice_upload($claim_id, 2, $pvc_upload_claim_doc_disabled);
    }
    if ($pvc_upload_ce_doc&01) {
        echo ncn_admin_draw_invoice_upload($claim_id, 10, $pvc_upload_ce_doc_disabled);
    }
    if ($pvc_upload_esx_file&01) {
        echo ncn_admin_draw_invoice_upload($claim_id, 11, $pvc_upload_esx_file_disabled);
    }
    if ($pvc_upload_ce_invoice_file&01) {
        echo ncn_admin_draw_invoice_upload($claim_id, 12, $pvc_upload_ce_invoice_file_disabled);
    }
    
    // get/setup data
//  $query = "SELECT * FROM claims_data WHERE claim_id=".$claim_id." AND field_type!='hidden' AND field_type!='submit' AND field_type!='token' ORDER BY `weight` ASC;";
    $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND field_type!=:b AND field_type!=:c AND field_type!=:d ORDER BY weight ASC',
    array(':a'=>$claim_id,':b'=>'hidden',':c'=>'submit',':d'=>'token'));
    $row_count = $result->rowCount();

    // page title
    drupal_set_title("Viewing claim: #".$claim_id." (".ncn_cd($claim_id, 'customer_name').")");

    
    // ---- draw data list ----
    ?>
    <?php if($pvc_claim_data&01): ?>
    <fieldset>
    <legend>Claim Data</legend>
    <form id="claims_data" method="post">
    <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    <input id="claims_data_update" name="claims_data_update" type="hidden" value="claims_data_update" />
    
    <table class="sticky-enabled tableSelect-processed sticky-table">
    <thead class="tableHeader-processed">
    <tr>
        <th>Data Key</th>
        <th>Data Value</th>
    </tr>
    </thead>

    <tbody>
    <?php
    
    drupal_add_js(drupal_get_path('module', 'ncn_image') . '/ncn_image.js');
    $index=0;
    //for ($i=0; $i<$row_count; $i++)
    foreach($result as $row)
    {
        // get the data
        $row = (array)$row;
        $row['val'] = unserialize($row['val']);
        $res = "";

        if (!isset($row['val']['type']))
        {   $row['val']['type'] = "scope";  }
        
        // convert the data
        switch ($row['val']['type'])
        {
            case "checkboxes":
                $res = "";
                
                foreach ($row['val'] as $k=> $v)
                {
                    if (trim($v) != "0")
                    {   $res .= "<li> ".$v; }
                }
                
                if ($res != "")
                {   $res = "<ul>".$res."</ul>";     }
            break;
        
            case "phone_field_phone":
                $res = implode(" ", $row['val']);
            break;

            case "time_field_time":
                $res = render_claim_time($row['val']);
            break;

            case "date":
                $res = render_claim_date($row['val']);
            break;
        
            case "image_upload_element":
                $image_url = $base_url."/".$row['val']->filepath; 
                $img_info = image_get_info($row['val']->filepath);
                
                $res = "<a href='".$image_url."?width=".$img_info['width']."&height=".$img_info['height']."&iframe=false' class='colorbox-load'>"; 
                $res .= "<img src='".str_replace('default/files', 'default/files/imagecache/ncnadmin', $image_url)."' />";
                $res .= "</a>"; 
                $res .= "<br /><small style='margin:0;padding:0;'>(click to enlarge)</small>";
            break;
            
            case "scope":
            break;
            default:
                //echo $row['key']."<br/>";
                if ($row['key'] == 'date_of_loss') {
                    /* Hack for Mobile App Data */
                    if((int)$row['val']['val'][1]==0){
                        $xchng_ary = array($row['val']['val'][1],$row['val']['val'][0],$row['val']['val'][2]);
                        $row['val']['val'] = $xchng_ary;
                    }
                    /* Hack for Mobile App Data */
                    $res = ncn_admin_render_date_loss($row['key'], $row['val']['val']);
                } else if ($row['key'] == 'insured_state') {
                    /* Hack for Mobile App Data */
                    preg_match('/\(([^\)]*)\)/', $row['val']['val'], $ins_match);
                    if(isset($ins_match[1]) && trim($ins_match[1])!=''){
                        $row['val']['val'] = $ins_match[1];
                    }
                    /* Hack for Mobile App Data */
                    $res = ncn_loc_state($row['key'], 'claim_data_field_'.$row['key'], false, false, $row['val']['val'], 'US');
                } else {
                    if (is_array($row['val']['val']))
                    {
                            $res = '<input name="'.$row['key'].'" value="'.implode(" ", $row['val']['val']).'" />';
                    }
                    else
                    {
                        $res = '<input name="'.$row['key'].'" value="'. $row['val']['val'].'" size="50"  />';
                        
                    }
                }
            break;
        }
        
        // display the data
        if ($res != "" && $row['key']!='id') {
        ?>
        <tr class="<?php if ($index%2) print "even"; else print "odd"; ?>">
            <td><?= ucwords(str_replace("_", " ", $row['key'])); ?></td>
            <td><?= $res; ?></td>
        </tr>
        <?php
            $index+=1;
        }
    }
    ?>
    <?php if($pvc_claim_data_disabled!='disabled'): ?>
    <tr class="<?php if ($index%2) print "even"; else print "odd"; ?>">
        <td colspan="2">
            <input type="submit" onclick="return on_set_scroll_position();" value="Save Claims Data" />
        </td>
    </tr>
    <?php endif; ?>
    </tbody>
    </table>
    </form>
    </fieldset>
    <?php endif; //Claim Data ?>
    
    <?php
    if ($pvc_claim_invoice_data&01) {
        echo ncn_admin_invoice_data($claim_id, $invoice_data, $pvc_claim_invoice_data_disabled);
    }
    
    if ($pvc_claim_invoice_doc&01) {
        echo ncn_admin_draw_invoice_upload($claim_id, 3, $pvc_claim_invoice_doc_disabled);
        echo ncn_admin_draw_invoice_upload($claim_id, 4, $pvc_claim_invoice_doc_disabled);
        echo ncn_admin_draw_invoice_upload($claim_id, 5, $pvc_claim_invoice_doc_disabled);
        echo ncn_admin_draw_multi_file_upload($claim_id, $pvc_claim_invoice_doc_disabled);
    }
    ?>
    </div>
    <?php if($pvc_photo_album&01): ?>
    
    <form id="ncn_admin_add_room" method="post">
        <input type="hidden" name="photo_add_room" value="photo_add_room" />
        <input type="hidden" id="room_name" name="room_name" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    </form>
    <form id="ncn_admin_delete_room" method="post">
        <input type="hidden" name="photo_delete_room" value="photo_delete_room" />
        <input type="hidden" id="room_name" name="room_name" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    </form>
    <form id="ncn_admin_delete_first_room" method="post">
        <input type="hidden" name="photo_delete_first_room" value="photo_delete_first_room" />
        <input type="hidden" id="room_name" name="room_name" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    </form>
    
    <table class="sticky-enabled tableSelect-processed sticky-table">
    <thead>
        <th>Photo Album<span style="margin-left:50px;"><a href="#ncn_admin_add_room" onclick="on_click_ncn_admin_add_room();">Add Room</a></span>
        </th>
    </thead>
    <tbody>
    <?php 
//      $query = "SELECT * FROM claims_data WHERE claim_id=".$claim_id." AND field_type!='hidden' AND field_type!='submit' AND field_type!='token' ORDER BY `weight` ASC;";
        $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND field_type!=:b AND field_type!=:c AND field_type!=:d ORDER BY weight ASC',
        array(':a'=>$claim_id,':b'=>'hidden',':c'=>'submit',':d'=>'token'));
    //   $result = db_query($query);
        if ($result == false) {
            $row_count = 0;
        } else {
            $row_count = $result->rowCount();
        }
        //mysql_data_seek($result, 0);
        for ($i=0; $i<$row_count; $i++)
        {
            // get the data
            $row = $result->fetchAssoc();
            if ($row['key'] == '#data') { continue; }
            
            $row['val'] = unserialize($row['val']);
            $res = "";
            if (!isset($row['val']['type']))
            {   $row['val']['type'] = "scope";  }   
        
            switch ($row['val']['type'])
            {
                case "scope":
                    $start_num = 0;
                    foreach ($row['val'] as $roomname => $roomdata)
                    {
                        if ($roomname != "type")
                        {
                            $first_room = false;
                            $default_room_name = ncn_admin_claim_get_default_room_name($claim_id);
                                                        
                            if ($roomname == $default_room_name) {
                                $first_room = true;
                            }
                            if ($first_room && ncn_admin_is_photo_first_room_deleted($claim_id)) { continue; }
                            
                            $res .= '<fieldset class="claims-roomdata clearfix">';
                            $res .= '<legend><h3><b>'.$roomname.'</b></h3></legend>';
                            $image_count = 6;
                            if ($first_room) {
                                $image_count = 3;
                            }
                            
                            $res .= '<form class="update-captions" method="post" >';
                            $res .= '<input type="hidden" name="roomname" value="'.$roomname.'" />';
                            $res .= '<input type="hidden" name="update_captions" value="update_captions" />';
                            $res .= '<input type="hidden" name="image_count" value="'.$image_count.'" />';
                            $res .= '<input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />';
                            $res .= '<div class="claims-room-images clearfix">';
                            
                            
                            $captions = _get_image_captions($claim_id, $roomname, $image_count);
                            for ($i=0; $i<$image_count; $i++)
                            //foreach ($row['val'][$roomname] as $img)
                            {
                                $img_index = 'image'.$i;
                                
                                $res .= '<div class="scope-image">';
                                if (isset($row['val'][$roomname][$img_index])) {
                                    $img = $row['val'][$roomname][$img_index];
                                    $image_url = $GLOBALS['base_url']."/".$img['path']."?rid=". date('U');


                                    $win_width = "800";
                                    $win_height = "620";
                                    $in_url = $GLOBALS['base_url']."/admin/config/ncn_image_open/?";
                                    $in_url .= "img=".urlencode($img['path'])."&";
                                    $in_url .= "claim_id=".$claim_id."&room_name=".$roomname . "&position=".$img_index."&";
                                    $in_url .= "width=".$win_width."&height=".$win_height;

                                    if ($pvc_photo_album_disabled != 'disabled') {
//                                      $res .= "<a onclick=\"open_image_box('".$GLOBALS['base_url']."', $claim_id, '$roomname', '$img_index', '".urlencode($img['path'])."')\" >";
                                        $res .= "<a class='colorbox-node' href='{$in_url}'  >";
                                    }
                                    
                                    if(strpos($image_url,"public://")!==false){
                                        $timage_url = str_replace('public://', 'sites/default/files/styles/image_tab_upload/public/', $image_url);                                      
                                    } else {
                                        $timage_url = str_replace('default/files', 'default/files/imagecache/ncnadmin', $image_url);                                        
                                    }
                                    // Show with D7 way
                                    {
                                        $t_path = str_replace('sites/default/files/', 'public://', $img['path']);
                                        $timage_url = image_style_url('medium',$t_path);
                                    }
                                    $res .= "<img src='".$timage_url."' />";
                                    if ($pvc_photo_album_disabled != 'disabled') {
                                        $res .= "</a>"; 
                                    }
                                    
                                } else {
                                    $image_url = _get_blank_room_image_url($image_count, $i);
                                    //$res .= '<div class="blank-image">&nbsp;</div>';

                                    $win_width = "800";
                                    $win_height = "620";
                                    $in_url = $GLOBALS['base_url']."/admin/config/ncn_image_open/?";
                                    $in_url .= "claim_id=".$claim_id."&room_name=".$roomname . "&position=".$img_index."&";
                                    $in_url .= "width=".$win_width."&height=".$win_height;

                                    if ($pvc_photo_album_disabled != 'disabled') {
//                                      $res .= "<a onclick=\"open_image_box('".$GLOBALS['base_url']."', $claim_id, '$roomname', '$img_index', '')\" >";
                                        $res .= "<a class='colorbox-node' href='{$in_url}'  >";
                                    }
                                    $res .= '<img src="'.$image_url.'" width="200px"/>';
                                    if ($pvc_photo_album_disabled != 'disabled') {
                                        $res .= "</a>"; 
                                    }
                                }
                                $res .= '<div class="image-caption"><input type="text" name="'.$img_index.'" value="'.$captions[$i].'" /></div>';
                                $res .= "</div>";
                                
                                /*$res .= "<a href='".$image_url."?width=500&height=500&iframe=false' class='colorbox-load'>"; 
                                $res .= "<img src='".str_replace('default/files', 'default/files/imagecache/ncnadmin', $image_url)."' />";
                                $res .= "</a>"; */
                            }
                            
                            $res.= "</div>";
                            
                            if ($pvc_photo_album_disabled != 'disabled') {
                                $res.= "<div class='update-caption-action'>";
                                $res.= '<input type="submit" onclick="return on_set_scroll_position();" value="Update Captions" />';
                                if ($image_count==6) {
                                    $res.= ('&nbsp;&nbsp;&nbsp;<input type="button" value="Edit Roomname" onclick="on_click_edit_roomname('.$claim_id.', \''.$roomname.'\')"/>');
                                    $res.= ('&nbsp;&nbsp;&nbsp;<input type="button" value="Delete Room" onclick="on_click_ncn_admin_delete_room(\''.$roomname.'\')"/>');
                                } else {
                                    $res.= ('&nbsp;&nbsp;&nbsp;<input type="button" value="Delete Room" onclick="on_click_ncn_admin_delete_first_room(\''.$roomname.'\')"/>');
                                }
                                $res.= "</div>";
                            }
                            $res .= '</form>';
                            
                            $res.= "<div class=''>";
                            if ($image_count==6) {
                                $res.= ncn_admin_draw_scope_upload($claim_id, "$roomname", $pvc_photo_album_disabled, $start_num);
                            }
                            $res.= "</div>";
                            
                            //* Edit Room ScopeSheet Link
                            
                            if ($image_count==6) {
                                $res.="<div class='claim-view-scopesheet-links clearfix'>";
                                if (user_access('ncn_admin view room scopesheet')) {
                                    $_room_ss_view_url = base_path()."admin/config/ncn_admin/view-room-scopesheet/$claim_id/$roomname";
                                    $res.= "<div class='edit-room-scopesheet-link room-ss-link'><a href='$_room_ss_view_url' target='_blank'>View Room ScopeSheet</a></div>";
                                }
                                if (user_access('ncn_admin edit room scopesheet')) {
                                    $_room_ss_edit_url = base_path()."admin/config/ncn_admin/edit-room-scopesheet/$claim_id/$roomname";
                                    $res.= "<div class='view-room-scopesheet-link room-ss-link'><a href='$_room_ss_edit_url' target='_blank'>Edit Room ScopeSheet</a></div>";
                                }
                                $res.="</div>";
                            }
                            
                            $res.= "</fieldset>";   // claims-roomdata
                            
                            $start_num ++;
                        }
                    }
                break;
            }
            if ($res != "") {
            ?>
            <tr class="<?php if ($i%2) print "even"; else print "odd"; ?>">
                <td><?= $res; ?></td>
            </tr>
            <?php
            }
        }
    ?>
    </tbody>
    </table>
    <?php endif; // Photo Album ?>
    
    
    <?php if (user_access('ncn change order request review')): ?>
    <div>
        <fieldset>
        <legend>Change Order Requests</legend>
        <div class='cor-lists'>
            <?php print ncn_change_order_render_request_list_table_admin($claim_id); ?>
        </div>
        </fieldset>
    </div>
    <?php endif; ?>
    
    <?php if ($pvc_create_invoice&01): ?>
    <?php echo ncn_admin_draw_invoice_upload($claim_id, 1, $pvc_create_invoice_disabled); ?>
    <?php if ($pvc_create_invoice_disabled!='disabled'): ?>
    <div class="create-invoice">
    <form id="create_invoice" method="post">
        <input type="hidden" name="create_invoice_function" value="create_invoice" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
        <input type="submit" onclick="return on_set_scroll_position();" value="CREATE INVOICE" />
    </form>
    </div>
    <?php endif; ?>
    <?php endif;//Create Invoice; ?>
    
    <?php
}

//------------------------------------------------------------------------------
function ncn_admin_download_pdf($claim_id)
{
    ncn_admin_download_xls($claim_id, true);
}

//------------------------------------------------------------------------------
// download an xls format document of all the claims data
function ncn_admin_download_xls($claim_id, $as_pdf = false)
{
GLOBAL $base_url;

    ob_end_clean();
    
    // include the php/xls class
    require_once 'sites/all/libraries/php_xls/Classes/PHPExcel.php';
     
    // Create new PHPExcel object
    $objPHPExcel = new PHPExcel();
    
    // Set properties
    $objPHPExcel->getProperties()->setCreator("Net Claims Now")
                                 ->setLastModifiedBy("Net Claims Now")
                                 ->setTitle("Office 2003 Document")
                                 ->setSubject("Office 2003 Document")
                                 ->setDescription("Claim data. Generated by Net Claims Now.")
                                 ->setKeywords("net claims now")
                                 ->setCategory("Claim Data");
    
    $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setWidth(37);
    $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setWidth(55);
    

    // insert standard data
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("A2", "Claim ID #");
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("B2", $claim_id);
    

    // get/setup data
//  $query = "SELECT * FROM claims_data WHERE claim_id=".$claim_id." AND field_type!='hidden' AND field_type!='submit' AND field_type!='token' ORDER BY `weight` ASC;";
    $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND field_type!=:b AND field_type!=:c AND field_type!=:d ORDER BY
     weight ASC',array(':a'=>$claim_id,':b'=>hidden,':c'=>submit,':d'=>token));
    $row_count = $result->rowCount();

    // page title
    
    // ---- draw data list ----
    $i = 0;

    for ($a=0; $a<$row_count; $a++)
    {

        $row = $result->fetchAssoc();

        // get the key
        $key = ucwords(str_replace("_", " ", $row['key']));
        $objPHPExcel->setActiveSheetIndex(0)->setCellValue("A".($i+5), $key);
    
        // get the data
        $row['val'] = unserialize($row['val']);
        $res = "";
        
        
        if (!isset($row['val']['type']))
        {   $row['val']['type'] = "scope";      }
        
        // convert the data
        switch ($row['val']['type'])
        {
            case "checkboxes":
                foreach ($row['val'] as $k=>$v)
                {
                    if ($v == "0")
                    {   unset($row['val'][$k]); }
                }
            
                $res = implode(", ", $row['val']);
                $objPHPExcel->setActiveSheetIndex(0)->setCellValue("B".($i+5), $res);
            break;
        
            case "phone_field_phone":
                $res = implode(" ", $row['val']);
                $objPHPExcel->setActiveSheetIndex(0)->setCellValue("B".($i+5), $res);
            break;

            case "time_field_time":
                $res = render_claim_time($row['val']);
                $objPHPExcel->setActiveSheetIndex(0)->setCellValue("B".($i+5), $res);
            break;

            case "date":
                $res = render_claim_date($row['val']);
                $objPHPExcel->setActiveSheetIndex(0)->setCellValue("B".($i+5), $res);
            break;
        
            case "scope":
            
                $count = 0;
            
                foreach ($row['val'] as $roomname => $roomdata)
                {
                
                    if ($count > 0)
                    {   $i++;   }
                
                    if ($roomname != "type")
                    {
                        $res .= '<h3>'.$roomname.'</h3>';
                        
                        $letters = array("B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M");
                        $k = 0;

                        foreach ($row['val'][$roomname] as $img)
                        {
                            // get path/image info
                            $image_url = $img['path'];
                            $img_path = str_replace('public://', 'sites/default/files/', $image_url);
                            if(!strpos($img_path, '.')){
                                $img_path .= '/'.$img['filename'];
                            }

                            if(file_exists(DRUPAL_ROOT.'/'.$img_path)){
                                $discard = file_get_contents($base_url."/".$img_path);
                                $img_info = image_get_info($img_path);

                                // change column height
                                $objPHPExcel->getActiveSheet()->getRowDimension(($i+5))->setRowHeight(($img_info['height']));
                                $objPHPExcel->getActiveSheet()->getColumnDimension($letters[$k])->setWidth(55);

                                // add image
                                $objDrawing = new PHPExcel_Worksheet_Drawing();
                                $objDrawing->setName($key);
                                $objDrawing->setDescription($key);
                                $objDrawing->setPath($img_path);
                                $objDrawing->setCoordinates($letters[$k].($i+5));
                                $objDrawing->setWorksheet($objPHPExcel->getActiveSheet());
                            }


                            $k++;


                        }
                    }
                    
                    $count++;
                }
                
            break;
        
            default:
            
                if (is_array($row['val']['val']))
                {   $res = implode(" ", $row['val']['val']);    }
                else
                {   $res = $row['val']['val'];      }
            
                $objPHPExcel->setActiveSheetIndex(0)->setCellValueExplicit("B".($i+5), $res, PHPExcel_Cell_DataType::TYPE_STRING);
                
//              $objPHPExcel->setActiveSheetIndex(0)->getCell('A'.($i+5))->setValueExplicit($res, PHPExcel_Cell_DataType::TYPE_TEXT);
            break;
        }
        
        $i++;

    }

    // close up the xls
    $objPHPExcel->getActiveSheet()->setTitle('Claim Data');
    
    // Set active sheet index to the first sheet, so Excel opens this as the first sheet
    $objPHPExcel->setActiveSheetIndex(0);


    // Redirect output to a client\92s web browser (Excel5)
    if ($as_pdf == false)
    {
        header('Content-Type: application/vnd.ms-excel');
        header('Content-Disposition: attachment;filename="claim-data_'.$claim_id.'.xls"');
        header('Cache-Control: max-age=0');
        $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
        $objWriter->save('php://output');
    }
    // Redirect output to a client\92s web browser (PDF)
    else
    {

        $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel2007');

        $tmp_xls_name = DRUPAL_ROOT.'/tmp/claim-data_'.$claim_id.'.xlsx';

        $tmp_pdf_name = DRUPAL_ROOT.'/tmp/claim-data_'.$claim_id.'.pdf';

        try{
            $objWriter->save($tmp_xls_name);
        }catch (Exception $e){
            print($e->getMessage());
            exit;
        }

        $command = "java -jar ../jod/lib/jodconverter-core-3.0-beta-3.jar ".$tmp_xls_name." ".$tmp_pdf_name;
        $output = shell_exec($command);

        echo $output;

        header('Content-type: application/pdf');
        header('Content-Disposition: attachment;filename="claim-data_'.$claim_id.'.pdf"');

        @ readfile($tmp_pdf_name);
        unlink($tmp_xls_name);
        unlink($tmp_pdf_name);
    }
    
    exit;
}


function ncn_admin_scopeform($claim_id, $roomname)
{
    ob_start();
?>

<p class="description" style="font-weight: bold;">
    If you don't upload scopesheet for final invoice, then we would generate this scopesheet using scopesheet data when creating a final invoice.
</p>

<form id="ncn_admin_scopeform_<?php echo $roomname; ?>" class="attach-description" enctype="multipart/form-data" method="post">
    <input type="file" size="60" id="ncn_scope_file_<?php echo $roomname; ?>" name="files[ncn_scope_file_<?php echo $roomname; ?>]" accept="image/jpg, image/jpeg, image/gif, image/png, image/bmp" />
    <input type="hidden" name="roomname" value="<?php echo $roomname; ?>" />
    <input type="hidden" name="scope_upload_function" value="scope" />
    <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    <input type="submit" onclick="return on_set_scroll_position();" value="Upload Exact Scope Image File" />
</form>
<p class="description">
    Upload cropped scope image file using this form. When creating invoice, uploaded image file is used, directly.
</p>

<form id="ncn_admin_auto_scopeform_pdf_<?php echo $roomname; ?>" class="attach-description" enctype="multipart/form-data" method="post">
    <input type="file" size="60" id="ncn_scope_auto_pdf_file_<?php echo str_replace(' ', '_', $roomname); ?>" name="files[ncn_scope_auto_pdf_file_<?php echo $roomname; ?>]" accept="application/pdf" />
    <input type="hidden" name="roomname" value="<?php echo $roomname; ?>" />
    <input type="hidden" name="scope_upload_function" value="scope_auto_full_pdf" />
    <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    <input type="submit" onclick="return on_set_scroll_position();" value="Upload Full Scope PDF File And Crop Automatically"  />
</form>
<p class="description">
    Upload full scope pdf file using this form. When uploading, scope image file would be cut from scope pdf, automatically.
</p>

<form id="ncn_admin_scopeform_pdf_<?php echo $roomname; ?>" class="attach-description" enctype="multipart/form-data" method="post">
    <input type="file" size="60" id="ncn_scope_pdf_file_<?php echo str_replace(' ', '_', $roomname); ?>" name="files[ncn_scope_pdf_file_<?php echo $roomname; ?>]" accept="application/pdf" />
    <!--<input type="hidden" name="roomname" value="<?php echo $roomname; ?>" />
    <input type="hidden" name="scope_upload_function" value="scope_full_pdf" /> -->
    <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
    <input type="button" value="Upload Full Scope PDF File And Crop Manually" onclick="return on_click_upload_scope_full_pdf('<?php echo $GLOBALS['base_url']; ?>', 'ncn_scope_pdf_file_<?php echo str_replace(' ', '_', $roomname); ?>', '<?php echo $claim_id; ?>', '<?php echo $roomname; ?>' ); " />
</form>
<p class="description">
    Upload full scope pdf file using this form. After uploading, you need to cut from scope file, manually.
</p>
<?php 
    $content = ob_get_contents();
    ob_end_clean();
    
    return $content;
}

function ncn_admin_scopeform_submit($roomname) {
    //$roomname = $form_state['values']['roomname'];
    $claim_id = arg(4);
    
    // we know the file was good
    $source = 'ncn_scope_file_'.$roomname;
    
    if(isset($form_state['clicked_button']['#post']['current_scroll_position']))
    {
        $_SESSION['current_scroll_position'] = $form_state['clicked_button']['#post']['current_scroll_position'];
    } 
    
    $file = file_save_upload($source);
    
    if ($file == 0 ) {
        drupal_set_message("File of missing for upload (room name :$roomname)", "error");
        return;
    } else if ($file->filemime != 'image/jpeg' && $file->filemime != 'image/jpg' && $file->filemime != 'image/gif' && $file->filemime != 'image/png' && strtolower(substr($file->filename, -4)) != ".bmp") {
        drupal_set_message("Scope file should be only Image (room name: $roomname)", "error");
        return;
    }

    $filePath = "sites/default/files/invoices/$claim_id/";
    $claim_dir = DRUPAL_ROOT."/".$filePath;
    
    if (!is_dir($claim_dir)) {
        mkdir($claim_dir, 0755);
    }

    $res = file_copy($file, "public://invoices/{$claim_id}", FILE_EXISTS_RENAME);

    // add invoice to database
    if ($res)
    {
        if ( strtolower(substr($file->filename, -4)) == ".bmp" ) {
            
//          $old_filepath = drupal_realpath($file->uri);
            $result_convert = convert_bmp_to_jpg($file);
            file_delete($file);
            if ($result_convert === FALSE) {
                drupal_set_message("Failed to convert uploaded bmp file to png file. ($roomname)", 'error');
                return;
            }
            $file = $result_convert;
        }else{
            $file = $res;
        }
    
        // get last revision
    //  $query = "SELECT * FROM claims_scope WHERE claim_id=".$claim_id." AND roomname=\"".$roomname."\"";
        $result = db_query('SELECT * FROM {claims_scope} WHERE claim_id=:a AND roomname=:b',
            array(':a'=>$claim_id,':b'=>$roomname));
        $row_count = $result->rowCount();
        
        
        if ($row_count == 0)
        {   
            $revision = 0;  
            $query = db_query('INSERT INTO {claims_scope} VALUES(NULL,:a,:b,:c,:d,:e,:f)',
                array(':a'=>$claim_id,':b'=>$roomname,':c'=>$file->filename,':d'=>$filePath.$file->filename,
                ':e'=>$file->filemime,':f'=>date('U')));
        }
        else
        {
            $row = (array)$result->fetchAssoc();
//          $revision = $row['revision']+1;

            $revision = 0;  
//          file_delete($row['filepath']);

            @unlink(drupal_realpath($row['filepath']));

            $query = db_query('UPDATE {claims_scope} SET filename=:a, filepath=:b, filemime=:d,
            `timestamp`=:e WHERE claim_id=:f AND roomname=:g',array(':a'=>$file->filename,':b'=>$filePath.$file->filename,
                ':d'=>$file->filemime,':e'=>date('U'),':f'=>$claim_id,':g'=>$roomname));
        }
        
        $result = $query;
        
        $log_message = t("!user_name uploaded Scope File of Room(!room_name). Filename: !file_name", array('!user_name'=>ncn_amin_get_user_role_and_name(), '!room_name'=>$roomname, '!file_name'=>$file->filename));
        ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
        // success!
        $log_message = t("Uploaded Scope File of Room(!room_name). Filename: !file_name", array('!room_name'=>$roomname, '!file_name'=>$file->filename));
        drupal_set_message('File "'.$file->filename.'" was uploaded (roomname: '.$roomname.').', 'status');
    } else {
        
    }
}

// upload scope pdf file and crop automatically.
function ncn_admin_auto_scopeform_full_pdf_submit($claim_id, $roomname) {
    
    // we know the file was good
    $source = 'ncn_scope_auto_pdf_file_'.$roomname;
    
    $file = file_save_upload($source);
    
    $json_data = array();
    $json_data['error'] = '';
    $json_data['roomname'] = $source;
    
    if ($file == 0 ) {
        drupal_set_message("File of missing for upload (room name :$roomname)", "error");
        return;
    } else if ($file->filemime != 'application/pdf') {
        drupal_set_message("Scope file should be only PDF (room name: $roomname)", "error");
        return;
    }
    
    if ($json_data['error'] == '') {

    $filePath = "sites/default/files/invoices/$claim_id/";
    $claim_dir = DRUPAL_ROOT."/".$filePath;
    
    if (!is_dir($claim_dir)) {
        mkdir($claim_dir, 0755);
    }

    $res = file_copy($file, "public://invoices/{$claim_id}", FILE_EXISTS_RENAME);

    $dest_file['uri'] = drupal_realpath($res->uri);
    $dest_file['filename'] = $res->filename;
    
    $img_file = convert_pdf_to_jpg($file, $claim_dir);
    if ($img_file == FALSE) {
        drupal_set_message(t("Failed to convert PDF to Image, after uploading scope pdf file(room name :!roomname)", array('!roomname'=>$roomname)), "error");
        return;
    } else {
    
        // Croping converted image
        $dest_file['uri'] .= "_cut.jpg";
        $dest_file['filename'] .= "_cut.jpg";
        $dest_file['filemime'] = "image/jpg";
        
        
        $api_params = array(
            'action'    => 'crop',
            'width'     => variable_get('ncn_scope_file_crop_width', '0'),
            'height'    => variable_get('ncn_scope_file_crop_height', '0'),
            'x'         => variable_get('ncn_scope_file_crop_x', '0'),
            'y'         => variable_get('ncn_scope_file_crop_y', '0'),
            
            'source'    => $img_file->uri,
            'dest'      => $dest_file['uri']
        );
            
        $bAction = _ncn_image_api('crop', $api_params);
        @unlink(drupal_realpath($img_file->uri));
        if ($bAction) {
            // get last revision
        //  $query = "SELECT * FROM claims_scope WHERE claim_id=".$claim_id." AND roomname=\"".$roomname."\"";
            $result = db_query('SELECT * FROM {claims_scope} WHERE claim_id=:a AND roomname=:b',
                array(':a'=>$claim_id,':b'=>$roomname));
            $row_count = $result->rowCount();
            
            if ($row_count == 0)
            {   
                $revision = 0;  
                $query = db_query('INSERT INTO {claims_scope} VALUES(NULL,:a,:b,:c,:d,:e,:f)',
                    array(':a'=>$claim_id,':b'=>$roomname,':c'=>$dest_file['filename'],
                    ':d'=>$filePath.$dest_file['filename'],':e'=>$dest_file['filemime'],':f'=>date('U')));
            }
            else
            {
                $row = (array)$result->fetchAssoc();
                //$revision = $row['revision']+1;
                
                $revision = 0;  

                @unlink(drupal_realpath($row['filepath']));

                $query = db_query('UPDATE {claims_scope} SET filename=:a , filepath=:b , filemime=:c , `timestamp`=:d WHERE claim_id=:e AND roomname=:f',
                array(':a'=>$dest_file['filename'],':b'=>$filePath.$dest_file['filename'],':c'=>$dest_file['filemime'],
                    ':d'=>date('U'),':e'=>$claim_id,':f'=>$roomname));
            }
      
            $result = $query;
            drupal_set_message('File "'.$dest_file['filename'].'" was generated (roomname: '.$roomname.').', 'status');
      
      $log_message = t("!user_name generated Scope File of Room !room_name. Filename: !file_name", array('!user_name'=>ncn_amin_get_user_role_and_name(), '!room_name'=>$roomname, '!file_name'=>$dest_file['filename']));
      ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
        } else {
            drupal_set_message(t("Failed to crop image, after uploading scope pdf file and converting to image (room name :!roomname)", array('!roomname'=>$roomname)), "error");
            return;
            
        }
    }
    }
    
}

// upload scope pdf file and crop manually.
function ncn_admin_scopeform_full_pdf_submit($claim_id, $roomname) {
    
    // we know the file was good
    $source = 'ncn_scope_pdf_file_'.$roomname;
    
    $file = file_save_upload($source);
    
    $json_data = array();
    $json_data['error'] = '';
    
    if ($file == 0 ) {
        //drupal_set_message("File of missing for upload (room name :$roomname)", "error");
        $json_data['error'] = "File of missing for upload (room name :$roomname)";
        //return;
    } else if ($file->filemime != 'application/pdf') {
        //drupal_set_message("Scope file should be only PDF (room name: $roomname)", "error");
        $json_data['error'] = "Scope file should be only PDF (room name: $roomname)";
        //return;
    }
    
    if ($json_data['error'] == '') {

    $filePath = "sites/default/files/invoices/$claim_id/";
    $claim_dir = DRUPAL_ROOT."/".$filePath;
    
    if (!is_dir($claim_dir)) {
        mkdir($claim_dir, 0755);
    }

    $res = file_copy($file, "public://invoices/{$claim_id}", FILE_EXISTS_RENAME);

    $dest_file['uri'] = drupal_realpath($res->uri);
    $dest_file['filename'] = $res->filename;
    
    $img_file = convert_pdf_to_jpg($file, $claim_dir);
    if ($img_file == FALSE) {
        //drupal_set_message(t("Failed to convert PDF to Image, after uploading scope pdf file(room name :!roomname)", array('!roomname'=>$roomname)), "error");
        $json_data['error'] = "Failed to convert PDF to Image, after uploading scope pdf file(room name :$roomname)";
        //return;
    } else {
    
        // Scaling converted image
        $dest_file['uri'] .= "_scope.jpg";
        $dest_file['filename'] .= "_scope.jpg";
        $dest_file['filemime'] = "image/jpg";
        
        
        $api_params = array(
            'action'    => 'scale',
            'width'     => 600,
            'height'    => NULL,
            'upscale'   => FALSE,
            
            'source'    => $img_file->filepath,
            'dest'      => $dest_file['uri']
        );
            
        $bAction = _ncn_image_api('scale', $api_params);
        @unlink(drupal_realpath($img_file->uri));
        if ($bAction) {
            // get last revision

            //drupal_set_message('File "'.$dest_file['filename'].'" was generated (roomname: '.$roomname.').', 'status');
            $json_data['filepath'] = base64_encode(drupal_realpath($dest_file['uri']));
        } else {
            //drupal_set_message(t("Failed to crop image, after uploading scope pdf file and converting to image (room name :!roomname)", array('!roomname'=>$roomname)), "error");
            $json_data['error'] = "Failed to crop image, after uploading scope pdf file and converting to image (room name :$roomname)";
            //return;
            
        }
    }
    }
    
    echo json_encode($json_data);
    
}

function ncn_admin_claimsdata_update($claim_id) {
    $error = false;
  $options = array();
    foreach($_POST as $key=>$val) {
        if (!is_array($val)) {
            $val = trim($val);
        }
    //  $query = "SELECT * FROM claims_data WHERE claim_id=$claim_id AND `key` = '$key' ";
        $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND `key`=:b',
            array(':a'=>$claim_id,':b'=>$key));
        if ($result->rowCount()>0) {
      $b_update = TRUE;
            // change the first room name
            if ($key == 'customer_name' && $val!= ncn_cd($claim_id, 'customer_name')) {
        $val = ncn_admin_remove_special_char_from_customer_name($val);
                if (ncn_admin_change_first_room_name($claim_id, $val)) {
          $options['insured_name'] = 'change';
        } else {
          $b_update = FALSE;
        }
            }
            
      if ($b_update) {
          $row = $result->fetchAssoc();
              $row['val'] = unserialize($row['val']);
              $row['val']['val'] = $val;
              $data = serialize($row['val']);
             // $query1 = "UPDATE claims_data SET `val` = '".mysql_real_escape_string($data)."' WHERE claim_id=$claim_id AND `key`='$key' ";
              $result1 = db_query('UPDATE {claims_data} SET val=:a WHERE claim_id=:b AND `key`=:c',
                  array(':a'=>$data,':b'=>$claim_id,':c'=>$key));
      }
        }
    }
    if (!$error) {
        drupal_set_message('Saved claims data, successfully.');
    }
  
  // Synchronize Additional Data
  ncn_admin_sync_additional_data($claim_id, $options);
}

//------------------------------------------------------------------------------
// upload user specific document
function ncn_admin_upload_document($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['op'] = array(
        '#type' => 'user_edit',
        '#value' => 'true',
    );

    $form['document_title'] = array(
        '#type' => 'textfield', 
        '#title' => t('Document Title'),
    );

    $form['document_upload'] = array(
        '#type' => 'file',
        '#title' => t(''),
    );
    
    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );
    
    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload Document'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}

//------------------------------------------------------------------------------
function ncn_admin_upload_document_validate($form, $form_state)
{

    // check title was long enough
    
    if (strlen($form_state['values']['document_title']) < 6)
    {
        // If you want to require it, you'll want to do it here... something like this:
        form_set_error('document_title', 'Document title must be at least 6 characters.');
        return;
    }

    // check file was uploaded
    $file = file_save_upload('document_upload');

    if (!$file)
    {
        // If you want to require it, you'll want to do it here... something like this:
        form_set_error('document_upload', 'File missing for upload.');
    }
  
}

//------------------------------------------------------------------------------
function ncn_admin_upload_document_submit($form, $form_state)
{
    $user_id = arg(4);

    if(isset($form_state['clicked_button']['#post']['current_scroll_position']))
    {
        $_SESSION['current_scroll_position'] = $form_state['clicked_button']['#post']['current_scroll_position'];
    } 

    // we know the file was good
    $file = file_save_upload('document_upload');
    $res = file_copy($file, 'sites/default/files/user_files/');
    
    // add invoice to database
    if ($res)
    {
    //  $query = "INSERT INTO member_documents VALUES(NULL, ".$user_id.", \"".AddSlashes($form_state['values']['document_title'])."\", \"".AddSlashes($file->filepath)."\", \"".AddSlashes($file->filename)."\",  \"".AddSlashes($file->filemime)."\");";
        $result = db_query('INSERT INTO {member_documents} VALUES(NULL,:a,:b,:c,:d,:e)',
            array(':a'=>$user_id,':b'=>$form_state['values']['document_title'],':c'=>$file->filepath,':d'=>$file->filename,
            ':e'=>$file->filemime));

        drupal_set_message('Uploaded new document.', 'status');
            
    }
    
}

//------------------------------------------------------------------------------
function ncn_admin_draw_scope_upload($claim_id, $roomname, $pvc_disabled='', $start_num) {
GLOBAL $base_url;
    ob_start();
?>
    <div class="scope_file">
        <fieldset class="">
            <legend class="">
            </legend>
            <div class="scope_form">
            <?php
                if ($pvc_disabled != 'disabled') {
                    print ncn_admin_scopeform($claim_id, $roomname );
                }
            ?>
            
                <div id="invoice_revisions">
                <table>
                <tbody>
                    <thead class="tableHeader-processed">
                    <tr>
                        <th>File</th>
                        <th>Timestamp</th>
                        <th>Action</th>
                    </tr>
                    </thead>
                    <?php
                    // get latest 5 invoice revisions
                    // $query = "SELECT * FROM claims_scope WHERE claim_id=".$claim_id." AND roomname =\"$roomname\"";
                    $result = db_query('SELECT * FROM {claims_scope} WHERE claim_id=:a AND roomname=:b',array(':a'=>$claim_id,':b'=>$roomname));
                    $row_count = $result->rowCount();
                    
                    // run through invoice revisions
                    for ($i=0; $i<$row_count; $i++)
                    {
                        $row = $result->fetchAssoc();
                        ?>              
                        <tr class="<?php if ($i%2) print "even"; else print "odd"; ?>">
                            <td><img src="<?php echo base_path() ?>/sites/all/modules/ncn_claims_manager/images/pdf_icon_16.gif" width="16" height="16" align="left" />&nbsp;<a href="<?= $base_url; ?>/account/serve_scope_file/<?= $claim_id; ?>/<?= $row['roomname']; ?>"><?= StripSlashes($row['filename']); ?></a></td>
                            <td><?= date("m/d/Y H:i:s", $row['timestamp']); ?></td>
                            <td>
                                <?php if ($pvc_disabled != 'disabled'): ?>
                                <a class="scope_file_<?=$i+$start_num*1000?>" href="<?= $base_url; ?>/account/delete_scope_file/<?= $claim_id; ?>/<?= $row['roomname']; ?>" onclick='set_scroll_position("scope_file_<?=$i+$start_num*1000 ?>"); return confirm("Are you sure you want to delete this file?");'>[delete]</a>
                                <?php endif; ?>
                            </td>
                        </tr>
                        <?php
                    }
                    
                    // no results
                    if ($row_count == 0)
                    {
                        ?>
                        <tr>
                            <td coslpan=3>No files uploaded</td>
                        </tr>
                        <?php
                    }                               
                ?>                  
                </tbody>
                </table>
                </div>
            </div>
        </fieldset>
            
        

    </div>
    <?php

    $content = ob_get_contents();
    ob_end_clean();
    
    return $content;
}

//------------------------------------------------------------------------------
// list the latest claims with sorting/filtering/etc 
function ncn_admin_list_claims()
{
    GLOBAL $user;
    GLOBAL $base_url;
    
    $tfunction = (isset($_REQUEST['tfunction']) ? $_REQUEST['tfunction'] : '');
    // Auto Assign Mode
    if ($tfunction == 'auto_assign_feature') {
        $aa_mode = ($_REQUEST['auto_assign_mode']==1)? TRUE:FALSE;
        if (ncn_ce_portal_set_mode($aa_mode)) {
            drupal_set_message(t('Auto Assign Mode: @mode', array( '@mode'=>($aa_mode)?'On':'Off' )));
        }
    } else if ($tfunction=='delete_multi_claims') {
    $sel_claims = $_REQUEST['sel_claims'];
    foreach ($sel_claims as $claim_id) {
      ncn_admin_delete_claim($claim_id);
    }
  }
    
  
    $filter = arg(4);
    $filter_user = arg(6);
    $filter_claim_workflow = arg(8);
    $filter_claim_id = arg(10);

  if (ncn_admin_is_user_DriRite($user->uid)) {
    $filter_user = 598; //Dustin Lawson (DriRite)
  }
  
    if (empty($filter_claim_workflow)) {
        $filter_claim_workflow = 'all';
    }
    
    if (empty($filter_claim_id)) {
        $filter_claim_id = 'all';
    }
    
    if (stristr($_SERVER['REQUEST_URI'], 'ncn_admin_pool'))
    {   $url_part = "ncn_admin_pool";   }
    else    
    {   $url_part = "ncn_admin";    }

    // ---- draw the filter controls ----
    // get claim status'
    $query = "SHOW COLUMNS FROM `claims` LIKE 'claim_status'";
    $result = db_query($query)->fetchAssoc();
    $row = (array)$result;
    
    $statuses = str_replace("'", "", $row['Type']);
    $statuses = str_replace("enum(", "", $statuses);
    $statuses = str_replace(")", "", $statuses);
    $statuses = str_replace("unpurchased,", "", $statuses);
    $statuses = explode(',', $statuses);
    array_unshift($statuses, "show all"); 
    
    // get active users
//  $query = "SELECT DISTINCT user_id FROM claims WHERE claim_status != 'unpurchased' AND deleted=0;";
    $result = db_query('SELECT DISTINCT user_id FROM claims WHERE claim_status != :a AND deleted=0',array(':a'=>'unpurchased'));
    $users = array();
    $result_rec = $result->fetchAll();
    foreach($result_rec as $row)
    {
        $row = (array)$row;
        if ( $row['user_id'] == 0 ) { continue; }
        $_user = user_load($row['user_id']);
        if ($_user) {
            $users[$row['user_id']] = $_user->profile_firstname." ".$_user->profile_lastname; 
            if ($_user->profile_legalname != '') {
                $users[$row['user_id']] .= (" (".$_user->profile_legalname.")");
            }
            if ($_user->profile_firstname == '' && $_user->profile_lastname == '' && $_user->profile_legalname == '') {
                 $users[$row['user_id']] = $_user->name;
            }
        }
    }
    
    asort($users);      // sort users by value (username)
    $users = array("show all" => "Show All", "show_members"=>"Show All Real Members")+$users;  


    ?>
    <div>
    <?php if (user_access('ncn auto-assign settings', $user)): ?>
    <fieldset>
        <legend>Auto Assign Claim to CE</legend>
        <form method="POST">
        <input type="hidden" name="tfunction" value="auto_assign_feature">
        <table>
        <tbody style="border:0;">
            <tr>
                <td style="width:50px;"><strong>Mode: </strong></td>
                <td style="width:50px;">
                    <?php $ce_auto_assign = variable_get('ce_auto_assign', FALSE); ?>
                    <select name="auto_assign_mode" id="auto_assign_mode">
                        <option value="0" <?php if (!$ce_auto_assign){echo "selected";} ?> >Off</option>
                        <option value="1" <?php if ( $ce_auto_assign){echo "selected";} ?>>On</option>
                    </select>
                </td>
                <td><input type="submit" value="Save"></td>
            </tr>
        </tbody>
        </table>
        </form>
    </fieldset>
    <?php endif; ?>
    
    <fieldset>
    <legend>Filter Claims</legend>
        <table>
        <tbody style="border:0;">
            <tr>
                <td style="width:150px;"><strong>Filter by Status</strong></td>
                <td>
                    <select name="status" id="filter_status_id">
                    <?php
                        foreach ($statuses as $status)
                        {
                            if ($status == $filter)
                            {   $checked = " selected ";    }
                            else
                            {   $checked = '';  }

                            ?>
                            <option value="<?= $status; ?>" <?= $checked; ?>><?= ucwords($status); ?></option>
                            <?php
                        }
                    ?>
                    </select>
                </td>
            </tr>
            <tr <?php if(ncn_admin_is_user_DriRite($user->uid)) { echo "style='display:none;'"; } ?>>
                <td><strong>Filter by User</strong></td>
                <td>
                    <select name="user" id="filter_user_id" >
                    <?php
                        foreach ($users as $user_id => $user_name)
                        {
                            if ($user_id == $filter_user)
                            {   $checked = " selected ";    }
                            else
                            {   $checked = '';  }

                            ?>
                            <option value="<?= $user_id; ?>" <?= $checked; ?>><?= $user_name ?></option>
                            <?php
                        }
                    ?>
                    </select>
                </td>
            </tr>
            <tr>
                <td><strong>Filter by Workflow</strong></td>
                <td>
                    <select id="filter_claim_workflow" name="claim_workflow">
                        <option value="all">Show All</option>
                    <?php
                    $arr_timer = get_claim_workflow_array();
                    foreach ($arr_timer as $key=>$val)
                    {
                        if ($key == $filter_claim_workflow)
                        {   $checked = " selected ";    }
                        else
                        {   $checked = '';  }
                    
                        ?>
                        <option value="<?= $key; ?>" <?= $checked; ?>><?= $val; ?></option>
                        <?php
                    }
                    
                    ?>
                    </select>
                </td>
            </tr>
            
            <tr>
                <td><strong>Filter by Claim ID</strong></td>
                <td><input type="text" class="form-text" value="" size="10" id="filter_claim_id" name="filter_claim_id" maxlength="10"></td>
            </tr>

            <tr>
                <td colspan=2><input type="button" value="Update" onclick="jump_status('<?= $base_url; ?>', '<?= $url_part; ?>');"></td>
            </tr>
        </tbody>
        </table>
    </fieldset>
    </div>
    <?php

    // ---- setup the query ----
    $query = "SELECT * FROM claims";
    $where = " WHERE claim_status != 'unpurchased' AND deleted=0";
    
    if ($filter != "show all")
    {   $where .= " AND claim_status='".$filter."' ";   }
    
    if ($filter_user == "show_members") {
        $where .= " AND user_id!=69";
    }
    else if (($filter_user != "show all") && ($filter == "show all"))
    {   $where .= " AND user_id='".$filter_user."' ";   }
    else
    if (($filter_user != "show all") && ($filter != "show all"))
    {   $where .= "AND user_id='".$filter_user."' ";    }
    
    if ($filter_claim_workflow != "all") {
        $where .= " AND workflow='".$filter_claim_workflow."' ";
    }
    
    if ($filter_claim_id != "all") {
        $where .= " AND claim_id='".$filter_claim_id."' ";
    }

    $query .= $where;
    $query .= " ORDER BY last_modified DESC";
    
    $query_total = db_query("SELECT COUNT(*) as tot FROM claims". $where)->fetchAssoc();
    $total_count = intval($query_total['tot']);

    // pagination
    $num_per_page = 50;
    $page = 0;
    if (isset($_GET['page'])) {
        $page = intval($_GET['page']);
    }
    $query .= " LIMIT ".($page*$num_per_page).", $num_per_page";
    
    $result = db_query($query);
    $row_count = $result->rowCount();
    if (user_access('ncn claim report')) {
        echo "<div style='float:left; margin-right: 20px;'><a href='$base_url/ncn/ncn_claim_report' target='_blank'>NCN Claim Report</a></div>"; 
    }
    if (user_access("ncn claim report for qa & ms")) {
        echo "<div style='float:left; margin-right: 20px;'><a href='$base_url/ncn/ncn_claim_report_qa_ms' target='_blank'>NCN Claim Report For QA&MS</a></div>"; 
    }
    if (user_access("ncn claim report for am")) {
        echo "<div style='float:left; margin-right: 20px;'><a href='$base_url/ncn/ncn_claim_report_am' target='_blank'>NCN Claim Report For Account Managers</a></div>"; 
    }
  
  echo "<div style='float:left; margin-right: 20px;'><a href='$base_url/download/claim_list'>Download Claim List</a></div>"; 
  
    echo '<div style="text-align: right">Number of Claims:&nbsp;&nbsp;'.$total_count.'</div>';
    // ---- list the claims ----
    
    $column_count = ($user->uid == 1)? 12: 11;
    $table_head = '
    <thead class="tableHeader-processed">
    <tr>
    <th></th>
        <th>Claim IDs</th>
        <th>Type</th>
        <th>Product</th>
        <th>Customer</th>
        <th>Zip Code</th>
        <th>User</th>
        <th>Created</th>
        <th>Modified</th>
        <th>Status</th>
        <th>Workflow</th>
        <th>Changes Logfile</th>'. (($user->uid == 1)? '<th></th>': '') .
    '</tr>
    </thead>

    <tbody>';

    $table_foot = '
    </tbody>
    </table>';
    
    // no results message
    if ($total_count == 0)
    {
        print "<table>";
        print $table_head;
        ?>
        <tr><td colspan="<?php echo $column_count; ?>">No results</td></tr>
        <?php
        print $table_foot;
    } else {
        echo "<form id='frm-check-mutli-claims' method='POST'>";
        echo '<table class="sticky-enabled tableSelect-processed sticky-table page_table">';
        echo $table_head;
        //for ($i=0; $i<$row_count; $i++)
        $result_rec = $result->fetchAll();
        $i = 0;
        foreach($result_rec as $row)
        {
            // get data
            $row = (array)$row;
            $claim_id = $row['claim_id'];
            $claim_type = $row['claim_type'];
      
            $claim_product = $row['claim_product']; 
            if ($claim_product=='Fire/Smoke Contents Cleaning') { $claim_product='Contents Cleaning'; }
            if ($claim_product=='Fire/Smoke Structure Cleaning'){ $claim_product='Structure Cleaning'; }
            if (ncn_cd($claim_id, 'expedite')) { $claim_product .= " (EXPEDITE)"; }
      
            $_user = user_load($row['user_id']);
            
            // check to see if this is owned (account manager is viewing)
            if (isset($GLOBALS['user']->roles[6]))
            {
                //* Account Manager 
            //  $query2 = "SELECT * FROM member_id_pool WHERE member_id=\"".$_user->profile_memberid."\";";
                $result2 = db_query('SELECT * FROM {member_id_pool} WHERE member_id=:a',array(':a'=>$_user->profile_memberid));
                $row_count2 = $result2->rowCount();
                
                if ($row_count2 == 0)
                {   $show_this = false; }
                else
                {
                    $row2 = $result2->fetchAssoc();
                    
                    
                //  $query2 = "SELECT parent FROM users_parent WHERE uid=".$row2['owner'];      // From Distributor, Get Account Manager
                    $result2 = db_query('SELECT parent FROM {users_parent} WHERE uid=:a',array(':a'=>$row2['owner']));
                    $row_count2 = $result2->rowCount();
                    $row2 = $result2->fetchAssoc();
                    
                    if ($row2['parent'] == $GLOBALS['user']->uid)
                    {   $show_this = true;      }
                    else    
                    {   $show_this = false;     }
                }
            }
            else
            {   $show_this = true;  }   
            
            // carry on
            if ($show_this == true || $show_this == false) // TEMP ** DISPLAY ALL CLAIMS
            {
            
                // carry on
            //  $query2 = "SELECT * FROM claims_log WHERE claim_id=".$claim_id.";";
                $result2 = db_query('SELECT * FROM {claims_log} WHERE claim_id=:a',array(':a'=>$claim_id));
                $entries_in_log = $result2->rowCount();
                
                
                // draw row
                ?>
                <tr class="<?php if ($i%2) print "odd"; else print "even"; ?>">
                    <td><?php if ($user->uid==1): ?><input type="checkbox" name="sel_claims[]" class='check-claim' value="<?php print $claim_id; ?>" /><?php endif; ?></td>
                    <td><a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/viewclaim/<?= $claim_id; ?>"><?= $claim_id; ?></a>
                        <?php if (ncn_admin_get_claim_first_free_locked($claim_id) == 'LOCKED') { echo '(Locked)'; } ?></td>
                    <td><?= ucwords($claim_type); ?></td>
                    <td><?= ucwords($claim_product); ?></td>
                    <td><?= ncn_cd($claim_id, 'customer_name'); ?></td>
                    <td><?= ncn_cd($claim_id, 'insured_zip'); ?></td>
                    <td><?= $_user->profile_firstname.' '.$_user->profile_lastname ?> (<?= $_user->name; ?>)</td>
                    <td><?= date("m/d/Y", $row['created']); ?></td>
                    <td><?= date("m/d/Y", $row['last_modified']); ?></td>
                    <td><?= ucwords($row['claim_status']); ?></td>
                    <td><?php 
                        $timer_trigger = render_claim_timer($claim_id, 1);
                        $workflow = render_claim_workflow($claim_id);
                        echo $workflow;
                        if ($timer_trigger != '') {
                            echo "<div>($timer_trigger)<div>";  
                        }
                        ?>
                    </td>
                    <td><a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/viewlog/<?= $claim_id; ?>"><?= $entries_in_log; ?> entries in log</a></td>
                    <?php if ($user->uid == 1) { ?>
                        <td><a href="<?= $base_url; ?>/admin/config/<?= $url_part; ?>/deleteclaim/<?= $claim_id; ?>?dest=<?php echo urlencode(request_uri()); ?>" onclick="return delete_claim(<?= $claim_id; ?>)">Delete</a></td>
                    <?php }?>
                </tr>
                <?php
            }           
            $i++;
        }
        echo $table_foot;
    if ($user->uid==1) {
      echo "<div class='claim-select-actions'>
        <input type='hidden' name='tfunction' id='tfunction_mutli_claims' value='' />
        <a href='#' onclick='return mutli_claims_action(\"delete_multi_claims\");'>Delete</a>
      </div>";
      echo "</form>";
      drupal_add_js("jQuery(document).ready(function() {
        jQuery('#frm-check-mutli-claims input.check-claim').each(function(index) {
          jQuery(this).bind('click', check_change_mutli_claim);
        });
        check_change_mutli_claim();
      });", 'inline');
    }
        $url = $base_url."/".drupal_get_path_alias($_GET["q"]);
        echo ncn_admin_table_pagniation($url, $total_count, $num_per_page, $page, 15);
    }
}

function ncn_admin_delete_claim($claim_id) {

    //$query = "DELETE FROM claims  WHERE claim_id =$claim_id";
    $query = "UPDATE claims SET deleted=1 WHERE claim_id =$claim_id AND deleted=0";
    $result = db_query($query);
    if ($result) {
        $msg_type = 'status';
        $msg = t('Claim(!claim_id) deleted, successfully', array('!claim_id'=>$claim_id));
    } else {
        $msg_type = 'error';
        $msg = t('Failed to delete claim(!claim_id).', array('!claim_id'=>$claim_id));
    }
    
    drupal_set_message($msg, $msg_type);
    
    /*$query = "DELETE FROM claims_data  WHERE claim_id =$claim_id";
    $result = db_query($query);
    
    $query = "DELETE FROM claims_invoice  WHERE claim_id =$claim_id";
    $result = db_query($query);
    
    $query = "DELETE FROM claims_log WHERE claim_id =$claim_id";
    $result = db_query($query);
    
    $query = "DELETE FROM claims_scope WHERE claim_id =$claim_id";
    $result = db_query($query);
    
    $query = "DELETE FROM claim_img_captions WHERE claim_id =$claim_id";
    $result = db_query($query);
    */
    // track delete of claims
    $timestamp = date('U');
    $query = "INSERT INTO track_delete_claim VALUES( NULL, $claim_id, $timestamp )";
    $result = db_query($query);
    
  if (isset($_REQUEST['dest'])) {
      $dest = $_REQUEST['dest'];
      $dest = urldecode($dest);
      drupal_goto($dest);
  }
}

function ncn_admin_delete_scope_file() {
GLOBAL $user;

    // get the user id for this claim
    $claim_id = arg(2);
    
    $user_id_sql = "";
    
    // get the "live"
    $roomname = arg(3);

    // get the revision
    
    
    //get current scroll position
    $scroll_position = $_GET['current_scroll_position'];
    
//  $query = "SELECT * FROM claims_scope WHERE `claim_id`=".$claim_id." AND `roomname`=\"".$roomname."\"";
    $result = db_query('SELECT * FROM {claims_scope} WHERE claim_id=:a AND roomname=:b',array(':a'=>$claim_id,':b'=>$roomname));
    if ($result->rowCount()>0) {
        $row = $result->fetchAssoc();
//      file_delete($row['filepath']);
        @unlink(drupal_realpath($row['filepath']));
        // get live/preview
    //  $query = "DELETE FROM claims_scope WHERE `claim_id`=".$claim_id." AND `roomname`=\"".$roomname."\"";
        $result = db_query('DELETE FROM {claims_scope} WHERE claim_id=:a AND roomname=:b',array(':a'=>$claim_id,':b'=>$roomname));
    }   
    // bounce back to claim
    
    header("Location:".base_path()."/admin/config/ncn_admin/viewclaim/".$claim_id."?current_scroll_position=".$scroll_position);
    
    exit;
}

function ncn_admin_serve_scope_file()
{
GLOBAL $user;

    // get the user id for this claim
    $claim_id = arg(2);
    
    // get the "live"
    $roomname = arg(3);
//  $query = "SELECT * FROM claims_scope WHERE `claim_id`=".$claim_id." AND `roomname`=\"".$roomname."\"";
    $result = db_query('SELECT * FROM {claims_scope} WHERE claim_id=:a AND roomname=:b',array(':a'=>$claim_id,':b'=>$roomname));
    if ($result->rowCount() == 0) {
        return("Access denied to scope file, or file not found.");
    }
    
    // get data
    $row = $result->fetchAssoc();
    
    // serve file
    header('Content-Type: '.$row['filemime']);
    header('Content-Disposition: attachment;filename="'.StripSlashes($row['filename']).'"');
    header('Cache-Control: max-age=0');

    readfile($row['filepath']); 

    exit;
}

function get_claim_file($claim_id, $live) {
//  $query = "SELECT * FROM claims_invoices WHERE claim_id=$claim_id AND live=$live ORDER BY timestamp DESC";
    $result = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=:b ORDER BY timestamp DESC',
    array(':a'=>$claim_id,':b'=>$live));
    if ($result && $result->rowCount()>0)
    {
        $row = $result->fetchAssoc();
        return $row;
    }
    return array(); 
}

function get_claim_scope_file($claim_id) {
//  $query = "SELECT * FROM claims_scope WHERE claim_id=$claim_id ORDER BY timestamp DESC";
    $result = db_query('SELECT * FROM {claims_scope} WHERE claim_id=:a ORDER BY timestamp DESC',array(':a'=>$claim_id ));
    if ($result)
    {   
        $row_count = $result->rowCount();
        $files = array();
        for ($i=0; $i<$row_count; $i++) {
            $row = $result->fetchAssoc();
            $files[$row['roomname']] = $row;
        }
        return $files;
    }
    return array(); 
}

function _add_pdf_invoice($file, $claim_id) {
    
    $live = 1;
    // get last revision
//  $query = "SELECT revision FROM claims_invoices WHERE claim_id=".$claim_id." AND `live`=".$live." ORDER BY revision DESC LIMIt 0,1";
    $result = db_query('SELECT revision FROM {claims_invoices} WHERE claim_id=:a AND live=:b ORDER BY revision DESC LIMIt 0,1',
    array(':a'=>$claim_id,':b'=>$live));
    $row_count = $result->rowCount();
    
    if ($row_count == 0)
    {   $revision = 0;  }
    else
    {
        $row = $result->fetchAssoc();
        $revision = $row['revision']+1;
    }
    
    // insert new data
//  $query = "INSERT INTO claims_invoices VALUES(NULL, ".$claim_id.", ".$revision.", \"".AddSlashes($file['filename'])."\", \"".AddSlashes($file['filepath'])."\", \"".AddSlashes($file['filemime'])."\", ".date('U').", ".$live.");";
    $result = db_query('INSERT INTO {claims_invoices} VALUES(NULL,:a,:b,:c,:d,:e,:f,:g)',
        array(':a'=>$claim_id,':b'=>$revision,':c'=>$file['filename'],':d'=>$file['filepath'],':e'=>$file['filemime'],
            ':f'=>date('U'),':g'=>$live));
    
    // add entry to logfile
    if ($live == 1)
    {   $log_message = ncn_amin_get_user_role_and_name()." created new file: ".$file->filename.", revision #".$revision;    }
    else
    {   $log_message = ncn_amin_get_user_role_and_name()." created new file: ".$file->filename.", revision #".$revision;    }
    
    ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
}
/**
Generate Final Invoice
*/
function generate_final_invoice($claim_id) {
    $final_invoice_name = "";

    $dir_url = variable_get('file_directory_path', 'sites/default/files') . "/invoices/$claim_id/";
    
    if (!is_dir($dir_url)) {
        mkdir($dir_url, 0755);
    }
    
    $file3_info = get_claim_file($claim_id, 3);     // Service Contract
    $file4_info = get_claim_file($claim_id, 4);     // Certificate of Completion
    $file5_info = get_claim_file($claim_id, 5);     // Completed Invoice
        
    $base_abs_path = $_SERVER['DOCUMENT_ROOT'];
    $foption = "";
    $error = false;
    try{
        // 1st pdf
        $first_pdf_url = _get_first_pdf($claim_id, $dir_url);
        if ($first_pdf_url == '[no-coversheet]') {
            // skip first pdf(cover sheet)
        }   else if ($first_pdf_url) {
            $foption .= " -f $base_abs_path/$first_pdf_url";
        } else {
            $error = true;
        }
        // && $_SERVER['REMOTE_ADDR']!='103.240.34.101'
        if (!$error) {
            // 2th pdf  (Completed Invoice)
            if (!empty($file5_info)) {
                $foption .= " -f $base_abs_path/".$file5_info['filepath'];
            }
            // 3rd pdf
            if (!empty($file3_info) || !empty($file4_info)) {
                $third_pdf_url = _get_third_pdf($file3_info, $file4_info, $dir_url);
                if (!empty($third_pdf_url)) {
                    $foption .= " -f $base_abs_path/$third_pdf_url";
                }
            }
            /*if (!empty($file3_info)) {
                $foption .= " -f $base_abs_path/".$file3_info['filepath'];
            }
            // 4th pdf
            if (!empty($file4_info)) {
                $foption .= " -f $base_abs_path/".$file4_info['filepath'];
            }*/
            
            // Additional Documents
            $additional_file_removed = TRUE;
            $pdf_file_additional_pdfs = _get_additional_docs_pdf($claim_id, $dir_url, $additional_file_removed);
            if ($pdf_file_additional_pdfs) {
                $foption .= " -f $base_abs_path/$pdf_file_additional_pdfs";
            }
            
            // 5th pdf
            $scope_pdf_count = 0;
            $pdf_file_url_pattern = _get_scope_pdf($claim_id, $dir_url, $scope_pdf_count);
            if ($scope_pdf_count) {
                for ($index = 0; $index<$scope_pdf_count; $index+=1) {
                    $pdf_file_url = $pdf_file_url_pattern.$index.".pdf";
                    $foption .= " -f $base_abs_path/$pdf_file_url";
                }
            }
        }
    } catch(Exception $ex) {
        drupal_set_message($ex->getMessage(), 'error');
        $error = true;
    }
    
    if (!$error) {
        
        $final_invoice_filename = 'invoice_'.date('U').'.pdf';
        $final_invoice_name = $dir_url.$final_invoice_filename;
        $out_option = " -o $base_abs_path/$final_invoice_name";
        
        //drupal_set_message($foption);
        
        /*if($_SERVER['REMOTE_ADDR']=="103.240.34.101"){
            echo $foption;
        }*/
        
                
        if ($foption != "") {
            //$command = "java -jar C:/pdfsam/lib/pdfsam-console-2.3.1e.jar $foption $out_option concat";
            $command = "java -jar ../pdfsam/lib/pdfsam-console-2.3.1e.jar $foption $out_option concat";
            
            $output = shell_exec($command);
            
            if (!is_file($final_invoice_name)) {
                drupal_set_message('Failed to create an invoice.', 'error');
                //drupal_set_message('You need to upload essential files to create an invoice.', 'error');
                $error = true;
            } else {
            
                if (ncn_admin_is_claim_first_free($claim_id)) {
                    // generate locked pdf // 2011-08-02
                    //$command = "java -jar C:/pdfwatermark/ncn_pdf_watermark.jar $base_abs_path/$final_invoice_name";
                    $command = "java -jar ../pdfwatermark/ncn_pdf_watermark.jar $base_abs_path/$final_invoice_name";

                    $output = shell_exec($command);
                    $final_locked_invoice_name = ncn_admin_get_locked_invoice_filename($final_invoice_name);
                    
                    if (!is_file($final_locked_invoice_name)) {
                        drupal_set_message('Failed to create an locked invoice.', 'error');
                    }
                }
            }
        }
        else{
            $error = true;
        }
    }
    
    // clear temporary files
    if ($first_pdf_url && $first_pdf_url!='[no-coversheet]') {  
        $first_pdf_file = new stdClass();
        $first_pdf_file->filepath = $pdf_file_url;
        $first_pdf_file->filename = basename($pdf_file_url);
        $first_pdf_file->uri = str_replace("sites/default/files/","public://",$pdf_file_url);
        //file_delete($first_pdf_url);
        file_delete($first_pdf_file);
    }
    
    if ($third_pdf_url) {   
        $third_pdf_file = new stdClass();
        $third_pdf_file->uri = str_replace("sites/default/files/","public://",$third_pdf_url);
        //file_delete($third_pdf_url);
        file_delete($third_pdf_file);
    }   
    
    if ($additional_file_removed && $pdf_file_additional_pdfs ) {
        $additional_pdf_file = new stdClass();
        $additional_pdf_file->uri = str_replace("sites/default/files/","public://",$pdf_file_additional_pdfs);
        //file_delete($pdf_file_additional_pdfs);
        file_delete($additional_pdf_file);
    }
    
    if ($scope_pdf_count) {
        for ($index = 0; $index<$scope_pdf_count; $index+=1) {
            $pdf_file_url = $pdf_file_url_pattern.$index.".pdf";
            $scope_pdf_file = new stdClass();
            $scope_pdf_file->uri = str_replace("sites/default/files/","public://",$pdf_file_url);
            //file_delete($pdf_file_url);
            file_delete($scope_pdf_file);
        }
    }
    
    if ($error) { return; }
    
    drupal_set_message("Final Invoice Created, Successfully.");
    drupal_set_message("File : $final_invoice_name");
    if (is_file($final_locked_invoice_name)) {
        drupal_set_message("Locked Invoice File : $final_locked_invoice_name");
    }
    
    $file_info = array(
        'filename' => $final_invoice_filename,
        'filepath' => $final_invoice_name,
        'filemime' => 'application/pdf'
    );
    _add_pdf_invoice($file_info, $claim_id);
}

function ncn_admin_get_locked_invoice_filename($fname) {
    return substr($fname, 0, -4)."_locked.pdf";
}

function _get_format_phone_string($str) {
    if (trim($str) == "") {
        return '';
    }
    $str = str_replace('-', '', $str);
    $str1 = substr($str, 0, 3);
    $str2 = substr($str, 3, 3);
    $str3 = substr($str, 6);
    $str = "$str1-$str2-$str3";
    
    return $str;
}

function _get_format_tax_id_string($str) {
    if (trim($str) == "") {
        return '';
    }
    $str = str_replace('-', '', $str);
    $str1 = substr($str, 0, 2);
    $str2 = substr($str, 2);
    $str = "$str1-$str2";
    
    return $str;
}

function _get_first_pdf($claim_id, $dir_url) {
    
    $pdf_file_url = $dir_url.time()."_first.pdf";
    //$template_first_page = "invoice_cover_sheet.doc";
    
    //get essential data
    $invoice_data = get_extended_data($claim_id);   
    $claim_invoice_type = $invoice_data['claim_invoice_type'];

    if ( strpos($claim_invoice_type , "*no*") !== false ) {
        return '[no-coversheet]';
    }
    
    $claim_data = get_claim_data($claim_id);
    $_user = user_load($claim_data['user_id']);
    $data = array();
    
    $data['company_name']           = $_user->profile_legalname;    //'ABC Restoration, Inc';
    $data['tax_id_number']          = $_user->profile_taxid;        //'12-3456789';
    
    $data['company_address1']       = $_user->profile_address;      //'123 Company Lane';
    $data['company_address2']       = $_user->profile_city.', '.$_user->profile_state.' '.$_user->profile_zip;      //  'Dallas, TX. 75201';
    $data['company_office_phone']   = _get_format_phone_string($_user->profile_officephone);    //'555-555-1234';
    
    $data['client_name']            = ncn_cd($claim_id, 'customer_name');   //'Robert Smith';
    $data['property1']              = ncn_cd($claim_id, 'insured_address'); //'123 Company Lane';
    $data['property2']              = ncn_cd($claim_id, 'insured_city').', '.ncn_cd($claim_id, 'insured_state').' '.ncn_cd($claim_id, 'insured_zip');       //'Dallas, TX. 75201';
    $data['home_phone']             = _get_format_phone_string(ncn_cd($claim_id, 'insured_phone_number'));
    
    $data['date_of_loss']           = ncn_cd($claim_id, 'date_of_loss');    //'12/07/10';
    
    $data = array_merge($invoice_data, $data);
    
    $logo = ncn_admin_get_member_logo_info($_user->profile_memberid);
    $b_logo = true;
    if (empty($logo)) { $b_logo = false; }
    $result = ncn_admin_make_cover_sheet($pdf_file_url, $data, $b_logo);

    if(is_file($pdf_file_url)) {
        
        /********* add Member Logo into coversheet page ************/
        if (empty($logo)) {
            return $pdf_file_url;
        }
        $file = new stdClass();
        $file->filepath = $pdf_file_url;
        $file->filename = basename($pdf_file_url);
        $file->filemime = "application/pdf";        
        $file->uri = str_replace("sites/default/files/","public://",$file->filepath);
        
        $logo_params = array(
                'x' => 100,
                'y' => 50,
                'width' => 350,
                'height' => 150,
                'valign' => 'bottom',
            );
            
        $final_cv_pdf = generate_pdf_with_member_logo($_user->profile_memberid, $file, $logo_params);

        file_delete($pdf_file_url);
        if ($final_cv_pdf) {
            return $final_cv_pdf;
        } else {
            drupal_set_message('Failed to make a cover sheet with logo.', 'error');
        }
    } else if ($result !== FALSE) {
        drupal_set_message('Failed to make a cover sheet.', 'error');
    }
    
    
    return '';
    
}

function _get_third_pdf($file3, $file4, $dir_url) {
    $both_image = 0;
    if (!empty($file3['filepath']) && is_file($file3['filepath']) ) {
        if ( strtolower(substr($file3['filepath'], -4)) == ".jpg" ) {
            $both_image += 3;
        }
    }
    if (!empty($file4['filepath']) && is_file($file4['filepath']) ) {
        if ( strtolower(substr($file4['filepath'], -4)) == ".jpg" ) {
            $both_image += 4;
        }
    }
    if ( $both_image==0 ) {
        return '';
    }
    
    $pdf_file_url = $dir_url.time()."_third.pdf";
    
    require_once('sites/all/libraries/PDFMerger/fpdf/pdf.php');
    $pdf=new PDF();
    $pdf->AddPage();
    if ($both_image == 7) {
        $pdf->Image($file3['filepath'], 0, 20, 120);
        $pdf->Image($file4['filepath'], 120, 20, 80);
    } else if ($both_image == 3) {
        $pdf->Image($file3['filepath'], 0, 20, 200);
    } else if ($both_image == 4) {
        $pdf->Image($file4['filepath'], 0, 20, 200);
    }
    
    $pdf->Output($pdf_file_url, 'F');
    
    if(is_file($pdf_file_url)) {
        return $pdf_file_url;
    } else {
        drupal_set_message('Failed to make a \"Service Contact & Certificate of Completion" page.', 'error');
    }
    return '';
}

function _get_scope_pdf($claim_id, $dir_url, &$pdf_count) {
    $scope_file = get_claim_scope_file($claim_id);
    
//  $query = "SELECT * FROM claims_data WHERE claim_id=".$claim_id." AND `key`='scope'";
    $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND `key`=:b',array(':a'=>$claim_id,':b'=>scope));
    $row_count = $result->rowCount();
    
    if ($row_count == 0) { return ''; }
    
    // get the data
    $row = $result->fetchAssoc();
    $data = unserialize($row['val']);
    
    $customer_name = ncn_cd($claim_id, 'customer_name');
    
    require_once('sites/all/libraries/PDFMerger/fpdf/pdf.php');
    
    $pdf=new PDF();
  
    $pdf_count = 0;
    $room_count = 0;
    
    foreach ($data as $roomname => $roomdata)
    {
        if ($roomname != "type")
        {
            $room_count += 1;
        }
    }
    
    $scope_per_pdf = 5;
    $pdf_count = 0;
    $page_count = 0;
    $pdf_file_url_pattern = $dir_url.time()."_scope_";
    $i = 0;
    
    // search first room and render
    if (!ncn_admin_is_photo_first_room_deleted($claim_id)) {
    foreach ($data as $roomname => $roomdata)
    {
        if ($roomname != "type")
        {
            $default_room_name = ncn_admin_claim_get_default_room_name($claim_id);
            if ($roomname == $default_room_name) {
                $first_flag = true;
            }
            
            if ($first_flag) {
                if ($i % $scope_per_pdf == 0 && $page_count) {
                    if ($pdf) {
                        $pdf_file_url = $pdf_file_url_pattern.$pdf_count.".pdf";
                        $pdf->Output($pdf_file_url, 'F');
                        
                        $pdf_count += 1;
                    }
                    $pdf=new PDF();
                    $page_count = 0;
                }
            
        if (!ncn_admin_invoice_check_default_room_data_exist($claim_id, $roomdata)) { break; }
                $pdf->AddPage();    
                $page_count++;
        
                $roomdata['captions'] = _get_image_captions($claim_id, $roomname, 3);
                $pdf->Header_Invoice($customer_name);           
                $pdf->Photo_Table_Invoice($roomdata);
                
                $i += 1;
                break;
            }
        }
    }
    }
    
    // other rooms
    foreach ($data as $roomname => $roomdata)
    {
        if ($roomname != "type")
        {
            $first_flag = false;
            $default_room_name = ncn_admin_claim_get_default_room_name($claim_id);
            if ($roomname == $default_room_name) {
                $first_flag = true;
            }
            
            if(!$first_flag) {
                if ($i % $scope_per_pdf == 0 && $page_count) {
                    if ($pdf) {
                        $pdf_file_url = $pdf_file_url_pattern.$pdf_count.".pdf";
                        $pdf->Output($pdf_file_url, 'F');
                        
                        $pdf_count += 1;
                    }
                    $pdf=new PDF();
                    $page_count = 0;
                }
            
                if (!ncn_admin_invoice_check_room_data_exist($claim_id, $roomname, $roomdata)) { continue; }
                $pdf->AddPage();    
                $page_count++;
        
                $roomdata['captions'] = _get_image_captions($claim_id, $roomname, 6);
                $pdf->Header_Invoice($customer_name." - ".$roomname);           
                $top2 = $pdf->RoomPhoto_Table_Invoice($roomdata);
                if (isset($scope_file[$roomname])) {
                    $pdf->draw_scope_file($scope_file[$roomname], $top2);
                } else {
                    /** Dry Log ScopeSheet**/
                    $ss_data = array();
                    $ss_eit = ncn_admin_get_ss_eit_list($claim_id, $roomname);
                    $ss_smr = ncn_admin_get_ss_smr_list($claim_id, $roomname); 
                    $ss_ep  = ncn_admin_get_ss_ep_list($claim_id, $roomname);
                    $ss_smr_standard    = ncn_admin_get_ss_structural_moisture_readings($claim_id, $roomname);
                    ncn_admin_array_trim($ss_smr_standard);
                    
                    if (!empty($ss_eit)) { $ss_data['eit'] = $ss_eit; }
                    if (!empty($ss_smr)) { $ss_data['smr'] = $ss_smr; }
                    if (!empty($ss_ep )) { $ss_data['ep']  = ncn_admin_ss_ep_print_format_data($ss_ep);  }
                    if (!empty($ss_smr_standard)) { $ss_data['smr_standard'] = $ss_smr_standard; }
                    
                    if (!empty($ss_data)) {
                        $pdf->SetY($top2);
                        $pdf->InsertScopesheetData($ss_data);
                    }
                }
                
                $i += 1;
            }
        }
    }
    
    
    if ($pdf && $page_count) {
        $pdf_file_url = $pdf_file_url_pattern.$pdf_count.".pdf";
        $pdf->Output($pdf_file_url, 'F');       
        $pdf_count += 1;
    }
    
    //$scope_link = '<a href="http://www.netclaimsnow.com/'.$pdf_file_url.'" target="_blank">Scopesheet PDF</a>';
    //drupal_set_message(t($scope_link), 'status');
    return $pdf_file_url_pattern;
}

function ncn_admin_invoice_check_default_room_data_exist($claim_id, $roomdata) {
  $ret = FALSE;
  for ($i=0; $i<3; $i++) {
    $img_index = 'image'.$i;
    if (isset($roomdata[$img_index])&&is_file($roomdata[$img_index]['path'])) {
      $ret = TRUE; break;
    }
  }
  return $ret;
}
function ncn_admin_invoice_check_room_data_exist($claim_id, $roomname, $roomdata) {
    //* photo album//
    $ret = FALSE;
    for ($i=0; $i<6; $i++) {
        $img_index = 'image'.$i;
        if (isset($roomdata[$img_index])&&is_file($roomdata[$img_index]['path'])) {
            $ret = TRUE; break;
        }
    }
    
    //ScopeSheet
    if (!$ret) {
        $scope_file = get_claim_scope_file($claim_id);
        if (isset($scope_file[$roomname])) {
            $ret = TRUE;
        } else {
            $ss_data = array();
            $ss_eit = ncn_admin_get_ss_eit_list($claim_id, $roomname);
            $ss_smr = ncn_admin_get_ss_smr_list($claim_id, $roomname); 
            $ss_ep  = ncn_admin_get_ss_ep_list($claim_id, $roomname);
            $ss_smr_standard    = ncn_admin_get_ss_structural_moisture_readings($claim_id, $roomname);
            ncn_admin_array_trim($ss_smr_standard);
            
            if (!empty($ss_eit)) { $ss_data['eit'] = $ss_eit; }
            if (!empty($ss_smr)) { $ss_data['smr'] = $ss_smr; }
            if (!empty($ss_ep )) { $ss_data['ep']  = ncn_admin_ss_ep_print_format_data($ss_ep);  }
            if (!empty($ss_smr_standard)) { $ss_data['smr_standard'] = $ss_smr_standard; }
            
            if (!empty($ss_data)) {
                $ret = TRUE;
            }
        }
    }
    return $ret;
}

function ncn_admin_array_trim($data){
    foreach ($data as $key=>$item) {
        if (empty($item)) {
            unset($data[$key]);
        } else if (is_array($item)) {
            ncn_admin_array_trim($data[$key]);
        }
    }
}

function save_extended_data($claim_id) {
    $data = array();
    $data['claim_invoice_type'] = $_POST['claim_invoice_type'];
    $data['company_fax_number'] = $_POST['company_fax_number'];
    $data['data_entered']       = $_POST['data_entered'];
    $data['price_list']         = $_POST['price_list'];
    $data['iop_type']           = $_POST['iop_type'];
    $data['iop_number']         = $_POST['iop_number'];
    $data['type_of_loss']       = $_POST['type_of_loss'];
    $data['insurance_company']  = $_POST['insurance_company'];
    $data['claim_number']       = $_POST['claim_number'];
    $data['claim_adjuster']     = $_POST['claim_adjuster'];
    $data['cap_type']           = $_POST['cap_type'];
    $data['claim_adjuster_phone']= $_POST['claim_adjuster_phone'];
    $data['claim_adjuster_fax'] = $_POST['claim_adjuster_fax'];
    $data['category']           = $_POST['category'];
    $data['start_date']         = $_POST['start_date'];
    $data['completion_date']    = $_POST['completion_date'];
    $data['cause']              = $_POST['cause'];
    
    $query = "DELETE FROM invoice_extended_data WHERE claim_id = $claim_id";
    $result = db_query($query); 

    foreach( $data as $fname=>$fval ) {
        if ($fval != '') {
            /*$query = "INSERT INTO invoice_extended_data VALUES(NULL, $claim_id, '$fname', '$fval')";
            $result = db_query($query);*/
            save_extended_data_item($claim_id, $fname, trim((string)$fval));
        }
    }
    drupal_set_message("Save invoice data, successfully.");
}

function save_extended_data_item($claim_id, $key, $value) {
    $result = db_query('SELECT * FROM {invoice_extended_data} WHERE claim_id=:a AND fname=:b',array(':a'=>$claim_id,':b'=>$key));
//  if ($result) && $row = db_fetch_array($result)) {
//      $result = db_query("UPDATE {invoice_extended_data} SET fvalue='%s' WHERE data_id=%d", $value, $row['data_id']);
//  }
    if($result->rowCount()>0)
    {
        foreach($result as $row)
        {
            $row = (array)$row;
            if($row)
            {
                $result = db_query('UPDATE {invoice_extended_data} SET fvalue=:a WHERE data_id=:b',array(':a'=>$value,':b'=>$row['data_id']));
            }
        }

    }
    else
    {
        $result = db_query('INSERT INTO {invoice_extended_data}(claim_id, fname, fvalue) VALUES(:a,:b,:c)', array(':a'=>$claim_id,':b'=>$key,':c'=>$value));
    }
}

function get_extended_data($claim_id) {
    $data = array();
//  $query = "SELECT * FROM invoice_extended_data WHERE claim_id = $claim_id";
    $result = db_query('SELECT * FROM {invoice_extended_data} WHERE claim_id=:a',array(':a'=>$claim_id));
    /*$row_count = $result->rowCount();
    for($i=0; $i<$row_count; $i++) */
    foreach($result as $row)
    {
        $row = (array)$row;
        $data[$row['fname']] = $row['fvalue'];
    }
    
    //* auto-populated
    if (!isset($data['company_fax_number']) || trim($data['company_fax_number']) == '') {
        $_user = ncn_admin_get_user_from_claim_id($claim_id);
        if ($_user) {
            $data['company_fax_number'] = $_user->profile_company_fax;
        }
    }
    return $data;
}

function save_image_captions($claim_id) {
    $roomname   = $_POST['roomname'];
    $img_count  = $_POST['image_count'];
    
    for ($i=0; $i<$img_count; $i++) {
        $img_index = 'image'.$i;
        $data[$i] = $_POST[$img_index];
    }
    
    if (_save_image_captions($claim_id, $roomname, $data)) {
        drupal_set_message("Image captions updated, successfully. (room:$roomname)");
    $log_message = ncn_amin_get_user_role_and_name()." updated Image Captions . (room:$roomname)";
    ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
    } else {
        drupal_set_message("Failed to update image captions. (room:$roomname)", 'error');
    }
    
}

function track_delete_claims_page() {
global $base_url;
    if (arg(4) == 'undelete') {
        undelete_claim_action(intval(arg(5)));
    }
    
    drupal_add_js(drupal_get_path('module', 'ncn_admin') . '/ncn_admin.js');
    
//  $query = "SELECT * FROM track_delete_claim ORDER BY timestamp DESC";
    $result = db_query('SELECT * FROM {track_delete_claim} ORDER BY timestamp DESC');
    $row_count = $result->rowCount();
    
    $table_head = '
    <thead class="tableHeader-processed">
        <th>Claim ID</th>
        <th>User</th>
        <th>Date</th>
        <th></th>
    </thead>
    <tbody>';

    $table_foot = '
    </tbody>
    </table>';
    
    ob_start();
    
    // no results message
    if ($row_count == 0)
    {
        print "<table>";
        print $table_head;
        ?>
        <tr><td colspan=2>No results</td></tr>
        <?php
        print $table_foot;
        
    }
    
    $count = 0;
    $num_per_page = 50;
    $on_page = 0;
    $current_page = 1;

    for ($i=0; $i<$row_count; $i++)
    { 
        // get data
        $row = $result->fetchAssoc();
        $claim_id = $row['claim_id'];
        
    //  $query = "SELECT * FROM claims WHERE claim_id=$claim_id";
        $result1 = db_query('SELECT * FROM {claims} WHERE claim_id=:a',array(':a'=>$claim_id));
        if(($claim_count=$result1->rowCount())>0) 
        {
            $claim = $result1->fetchAssoc();
            $_user = user_load($claim['user_id']);
            if ($claim['deleted']==0) {
                continue;
            }
        }
    // header?
        if ($on_page == 0)
        {
            if ($current_page == 1)
            {   ?><table class="sticky-enabled tableSelect-processed sticky-table page_table" id="page_table_<?= $current_page; ?>"><?php   }
            else
            {   ?><table class="sticky-enabled tableSelect-processed sticky-table page_table"  id="page_table_<?= $current_page; ?>" style="display:none;"><?php    }
                
            print $table_head;  
        }
    
        // draw row
        ?>
        <tr class="<?php if ($count%2) print "even"; else print "odd"; ?>">
            <?php if ($claim_count==0) : ?>
                <td><?= $claim_id; ?></td>
            <?php else: ?>
                <td><a href="#" onclick="toggle_tr('track_deleted_claim_<?php echo $claim_id;?>')"><?= $claim_id; ?></a></td>
            <?php endif; ?>
            <td><?php if($claim_count) { echo $_user->profile_firstname.' '.$_user->profile_lastname.' ('.$_user->name.')'; } ?></td>
            <td><?= date('d M Y H:i:s', $row['timestamp']); ?></td>
            <td>
                <?php if ($claim_count > 0) : ?>
                    <a href="<?= $base_url; ?>/admin/config/ncn_admin/track_delete_claims/undelete/<?php echo $claim_id?>">Undelete</a>
                <?php else: ?>
                    Undelete
                <?php endif; ?>
            </td>
        </tr>
        <?php if ($claim_count>0) : ?>
        <tr class="<?php if ($count%2) print "even"; else print "odd"; ?> hidden-tr">
            <td></td>
            <td colspan="2">
                <div id="track_deleted_claim_<?php echo $claim_id;?>" class="hidden-body" >
                    <?php echo draw_ncn_claim_data($claim_id); ?>
                </div>
            </td>
            <td></td>
        </tr>
        <?php endif; ?>
        
        <?php
        
        // update page count            
        $count++;
        $on_page++;
        
        // check end of page
        if ($on_page == $num_per_page)
        {
            ?>
            <?php               
        
            print $table_foot;
            
            $on_page = 0;
            $current_page += 1;
        }
    }
        
    if ($on_page == 0) {
        $current_page -= 1;
    }
    // end pagination
    ?>
    <table class="sticky-enabled tableSelect-processed sticky-table">
    <tbody>
    <tr>
        <td colspan="7" align="right">
            
            <?php
                if ($current_page > 1) {
                    echo t("Page:");
                
                    for ($i=1; $i<= $current_page; $i++)
                    {
                        if ($i > 1)
                        {   
                            print " | ";
                            $page_text = '<u>'.$i.'</u>';   
                        }
                        else
                        {   $page_text = '<strong><u>'.$i.'</u></strong>';  }   
                        
                        
                        ?>
                        <a href="#" onclick="change_list_page(<?= $i; ?>);" id="page_link_<?= $i; ?>"><?= $page_text; ?></a>
                        <?php
                    }
                }
                //$current_page;
            ?>
        </td>
    </tr>
    </tbody>
    </table>
    <?php
    
    // javascript list of page
    ?>
    <script type="text/javascript">
        var total_list_pages = <?= $current_page; ?>;
    </script>
    <?php   
    $content = ob_get_contents();
    ob_end_clean();
    
    return $content;
}

function draw_ncn_claim_data($claim_id) {
//  $query = "SELECT * FROM claims_data WHERE claim_id=".$claim_id." AND field_type!='hidden' AND field_type!='submit' AND field_type!='token' ORDER BY `weight` ASC;";
    $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND field_type!=:b AND field_type!=:c AND field_type!=:d ORDER BY weight ASC',
    array(':a'=>$claim_id,':b'=>'hidden',':c'=>'submit',':d'=>'token'));
    //$row_count = $result->rowCount();
    //var_dump($row_count);

    ob_start();
    // ---- draw data list ----
    ?>
    
    <table>
    <tbody style="border:solid 0px;">
    <?php
    
    //for ($i=0; $i<$row_count; $i++)
    foreach($result as $row)
    {
        // get the data
        //
        $row = (array)$row;
        $row['val'] = unserialize($row['val']);
        $res = "";

        if (!isset($row['val']['type']))
        {   $row['val']['type'] = "scope";  }       
        
    
        // convert the data
        switch ($row['val']['type'])
        {
            case "checkboxes":
                $res = "";
                
                foreach ($row['val'] as $k=> $v)
                {
                    if (trim($v) != "0")
                    {   $res .= "<li> ".$v; }
                }
                
                if ($res != "")
                {   $res = "<ul>".$res."</ul>";     }
            break;
        
            case "phone_field_phone":
                $res = implode(" ", $row['val']);
            break;

            case "time_field_time":
                $res = render_claim_time($row['val']);
            break;

            case "date":
                $res = render_claim_date($row['val']);
            break;
        
            case "image_upload_element":
                $image_url = $base_url."/".$row['val']->filepath; 
                $img_info = image_get_info($row['val']->filepath);
                
                $res = "<a href='".$image_url."?width=".$img_info['width']."&height=".$img_info['height']."&iframe=false' class='colorbox-load'>"; 
                $res .= "<img src='".str_replace('default/files', 'default/files/imagecache/ncnadmin', $image_url)."' />";
                $res .= "</a>"; 
                $res .= "<br /><small style='margin:0;padding:0;'>(click to enlarge)</small>";
            break;
            
            case "scope": break;
            default:
            
                if (is_array($row['val']['val']))
                {
                    $res = implode(" ", $row['val']['val']);
                }
                else
                {
                    $res = $row['val']['val'];
                    
                }
            break;
        }
        
        // display the data
        if ($res != "") {
        ?>
        <tr>
            <td width="300"><?= ucwords(str_replace("_", " ", $row['key'])); ?></td>
            <td><?= $res; ?></td>
        </tr>
        <?php
        }
    }
    ?>
    </tbody>
    </table>
    
<?php
    $content = ob_get_contents();
    ob_end_clean();
    return $content;
}

/*
 * Undelet Claim
 */
function undelete_claim_action($claim_id) {
//  $query = "DELETE FROM track_delete_claim WHERE claim_id=$claim_id";
    $result = db_query('DELETE FROM {track_delete_claim} WHERE claim_id=:a',array(':a'=>$claim_id));
    if (!$result) {
        handle_mysql_syntax_error('30110-NCN-ADMIN', $query);
        return FALSE;
    }
    
//  $query = "UPDATE claims SET deleted=0 WHERE claim_id=$claim_id AND deleted=1";
    $result = db_query('UPDATE {claims} SET deleted=0 WHERE claim_id=:a AND deleted=1',array(':a'=>$claim_id));
    if (!$result) {
        handle_mysql_syntax_error('30110-NCN-ADMIN', $query);
        return FALSE;
    }
    
    drupal_set_message(t('Claim(#%claim_id) undeleted, successfully.', array('%claim_id'=>$claim_id)));
    
    return TRUE;
}

/*
 *
 */
function ncn_admin_claim_file_note_change($claim_id) {
    $file_note = $_POST['claim_file_note'];
  $hid = ncn_capsulecrm_note_add_note($claim_id, $file_note);
  
    $file_note = base64_encode(strip_tags($file_note));
    $log_message = ncn_amin_get_user_role_and_name()." update notes on file to '".$_POST['claim_file_note']."' ";
    drupal_set_message($log_message, 'status');
  ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
}
