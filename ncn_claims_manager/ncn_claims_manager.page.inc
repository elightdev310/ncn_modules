<?php
//------------------------------------------------------------------------------
// complete user account edit page
function ncn_claims_manager_get_account_header_menu()
{
    global $user;
    $menu_data = menu_navigation_links('menu-account-header');
    if (isset($_SESSION['admin_sandbox']) && $_SESSION['admin_sandbox'] == $user->uid) {
        $menu_data['menu-310']['href'] = 'user/back_to_admin';
        $menu_data['menu-310']['title'] = 'Back to Admin';
    }
    return $menu_data;
}

/**
 * URL: account/my-account.html
 */
function ncn_edit_my_account()
{
    GLOBAL $user;

    ob_start();
    ?>
    <div id="my_account_page">
        <div class="map-content">
            <div class="map-header">
                <div class="title">Edit My Profile</div>
                <div class="help"><a href="#">Help</a></div>
            </div>
            <div class="map-content-wrapper">
                <?php user_edit_new(false, $user->uid); // function from ncn_admin.module ?>
            </div>
        </div>
    </div>
    <?php
    $contents = ob_get_contents();
    ob_end_clean();

    return $contents;

}

/**
 * URL: account/my-extra-profile.html
 */
function ncn_edit_extra_profile()
{
    GLOBAL $user;

    ob_start();
    ?>
    <div id="my_account_page">
        <div class="map-content">
            <div class="map-header">
                <div class="title">Edit My Extra Profile</div>
                <div class="help"><a href="#">Help</a></div>
            </div>
            <div class="map-content-wrapper">
                <?php user_edit_extra_profile($user->uid, false); // function from ncn_admin.module ?>
            </div>
        </div>
    </div>
    <?php
    $contents = ob_get_contents();
    ob_end_clean();

    return $contents;
}


/**
 * URL: account/confirm_submit_claim
 */
function ncn_claims_manager_submit_claim_confirm()
{
    GLOBAL $user, $custom_theme;

    // add the css file (always required for this)
//    drupal_add_css(drupal_get_path('module', 'ncn_claims_manager') . '/ncn_claims_manager.css', 'module', 'all', FALSE);
    drupal_add_css(drupal_get_path('module', 'ncn_claims_manager') . '/ncn_claims_manager.css', array('group' => CSS_DEFAULT, 'type' => 'file'));
    //drupal_add_js(drupal_get_path('module', 'ncn_claims_manager') . '/ncn_claims_manager.js');

    $custom_theme = "ncn_account";
    $GLOBALS['in_edit'] = true;
    $GLOBALS['claim_id'] = arg(2);

    return theme('ncn_confirm_submit_claim', array('stat'=>1));
}

//------------------------------------------------------------------------------
// preprocess function for "ncn_add_room.tpl.php"
function template_preprocess_ncn_confirm_submit_claim(&$vars)
{
}


//------------------------------------------------------------------------------
function ncn_claim_manager_homepage()
{
    ?>
    <div id="page_loading"></div>
    <div id="page_results" class="clearfix">
        <?php ncn_claim_manager_homepage_innerpage(); ?>
    </div>
    <?php
    // Splash Page
    if (module_exists('ncn_splash')) {
        ncn_splash_open_prompt_pages();
    }
}

// -----------------------------------------------
function ncn_claim_manager_homepage_innerpage()
{
    GLOBAL $user;
    global $base_url;
    if(in_array("leaduser",$user->roles)===true){
        $GLOBALS['account_role'] = "leaduser";
    } else if (in_array("associate",$user->roles)===true) {
        $GLOBALS['account_role'] = "associate";
    } else {
        $GLOBALS['account_role'] = "user";
    }
    ?>
    <div class="page-content clearfix">
        <script type="text/javascript">
            var reload_url = "<?php echo $base_url; ?>/account/update_claim_preview/account-home";
        </script>
        <div>

            <?php
            if ($GLOBALS['account_role'] != "leaduser") {
                ?>
                <div class="add-claim-section clearfix"><?php _ncn_claim_manager_homepage_add_claim(); ?></div>
            <?php } ?>



            <?php if ($GLOBALS['account_role'] == "leaduser") { ?>
                <div class="claim-list-lead clearfix">
                    <div class="my-active-claims list"><?php _ncn_claim_manager_homepage_my_active_claims(4); ?></div>
                    <div class="out-for-review list"><?php _ncn_claim_manager_homepage_out_for_reviews(4); ?></div>
                    <div class="receivables list"><?php _ncn_claim_manager_homepage_receivables(3) ?></div>
                </div>
            <?php } else { ?>
                <div class="claim-list clearfix">
                    <div class="my-active-claims list"><?php _ncn_claim_manager_homepage_my_active_claims(4); ?></div>
                    <div class="out-for-review list"><?php _ncn_claim_manager_homepage_out_for_reviews(4); ?></div>
                    <div class="receivables list"><?php _ncn_claim_manager_homepage_receivables(3) ?></div>
                </div>
            <?php } ?>


        </div>
    </div>
    <div class="sponsor-section">
        <div class="sponsor-section-content">
            <div class="title">OUR SPONSOR</div>
            <?php _ncn_claim_manager_homepage_sponsor() ?>
        </div>
    </div>
<?php
}

//----------------------------------------------------------------------

function _ncn_claim_manager_homepage_sponsor()
{
    echo ncn_sponsor_section();
}


function _ncn_claim_manager_homepage_add_claim()
{
    GLOBAL $user;
    global $base_url;
    if (isset($user->roles[23])) {
        $GLOBALS['account_role'] = "leaduser";
    } else if (isset($user->roles[7])) {
        $GLOBALS['account_role'] = "associate";
    } else {
        $GLOBALS['account_role'] = "user";
    }

    ?>
    <div class="ahome-section-wrapper">
        <!--        KT start        -->
        <div>
            <a href="<?php echo  $base_url ?>/account/create-new-claim.html" id="ahome-addclaim-top-btn"
               class="clearfix"><span>Add a Claim</span></a>

            <div id="bottom-text">To create a new claim click the button above</div>
        </div>
        <div>
            <div id="message-title">Messages and Alerts</div>
            <div id="message-body" style="<?php if ($GLOBALS['account_role'] == "leaduser") {
                echo 'height:365px;';
            } ?>">
                <h2><?= nl2br(variable_get('ncn_msg_alert_title', '')); ?></h2>

                <p><?= nl2br(variable_get('ncn_msg_alert_text', '')); ?></p>
            </div>
        </div>
        <div>
        </div>
    </div>
<?php
}

//----------------------------------------------------------------------
function _ncn_claim_manager_homepage_my_active_claims($min_row) {

// param
    $order_by = "last_modified";
    //$additional_sql = ' AND `claim_status`!="out for review" AND `claim_status`!="returned" AND `claim_status`!="approved" AND `claim_status`!="archived" ';
    $additional_sql = " AND claim_status= 'incomplete' ";
    $editable = true;
    $send_to_archive = false;
    $send_to_admin = true;
    $approve_deny = false;

    ?>

<?php if ($GLOBALS['account_role'] == "leaduser"){ ?>
<div class="ahome-section-wrapper-lead">
<?php }else{ ?>
    <div class="ahome-section-wrapper">
        <?php } ?>
        <div class="clearfix">
            <div class="title">My Active Claims</div>
            <div class="help"><a href="#">Help</a></div>
        </div>
        <div class="table clearfix">
            <div class="table-header">
                <table>
                    <tr>
                        <td width="120px">Customer Name</td>
                        <td width="100px">Photo Album</td>
                        <td width="100px">Claims Processing</td>
                        <td>Create My Invoice</td>
                    </tr>
                </table>
            </div>
            <div class="table-body">
                <table>
                    <?php
                    GLOBAL $user;
                    GLOBAL $base_url;

                    if ($editable == true) {
                        $_editable = 1;
                    } else {
                        $_editable = 0;
                    }

                    if ($send_to_admin == true) {
                        $_send_to_admin = 1;
                    } else {
                        $_send_to_admin = 0;
                    }

                    if ($approve_deny == true) {
                        $_approve_deny = 1;
                    } else {
                        $_approve_deny = 0;
                    }
                    //echo "<pre>";print_r($user->roles[23]);
                    if (isset($user->roles[23])) {
                        $query = "SELECT * FROM claims WHERE leaduser=" . $user->uid . " AND deleted=0 AND claim_status!='unpurchased' $additional_sql ORDER BY `$order_by` DESC limit 0, 4;";
                    } else {
                        $query = "SELECT * FROM claims WHERE user_id=" . $user->uid . " AND deleted=0 AND (claim_status!='unpurchased' ".$additional_sql.") ORDER BY `$order_by` DESC limit 0, 4;";
                    }
                    //echo $query;exit;
                    $result = db_query($query);                 
                    $row_count = $result->rowCount();
                    
                    $orders_total = 0;
                    $payment_received_total = 0;

                    $result_rec = $result->fetchAll();
                    foreach($result_rec as $row )
                    {
                        // get data from sql
                        $row = (array)$row;                     
                        $claim_id = $row['claim_id'];

                        // does the file note exists/not blank
                        if ($row['file_note'] == '') // give the file notes some default value
                        {
                            $row['file_note'] = base64_encode('[no note]');
                        }


                        // does an invoice exist?
                     //   $query2 = "SELECT * FROM claims_invoices WHERE claim_id=" . $claim_id . " AND `live`=2;";
                        $result2 = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=2',
                            array(':a'=>$claim_id));
                        $live_invoice_count = $result2->rowCount();

                       // $query2 = "SELECT * FROM claims_invoices WHERE claim_id=" . $claim_id . " AND `live`=1;";
                        $result2 = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=1',
                        array(':a'=>$claim_id));
                        $live_claim_count = $result2->rowCount();

                        // draw the row
                        ?>
                        <tr>
                            <td width="120px"><?php echo ncn_cd($claim_id, 'customer_name'); ?></td>

                            <?php // edit controls
                            if ($editable == true && !isset($user->roles[23])) {
                                ?>
                                <td width="100px">
                                    <?php    if (($row['claim_status'] != "receivables") && ($row['claim_status'] != "paid in full")) {
                                        /*if ( ($row['claim_status'] == 'out for review') || ($row['claim_status'] == 'approved'))  {*/
                                        $disabled_string = 'enabled';
                                    } else {
                                        $disabled_string = 'disabled';
                                    }
                                    ?>
                                    <div class="edit-album-btn clearfix">
                                        <a href="#"
                                           class="<?php echo $disabled_string; ?>" <?php if ($row['claim_status'] == 'out for review') print 'disabled'; ?> <?php if ($disabled_string != 'disabled' && $row['claim_status'] != 'out for review') { ?> onclick='open_edit_box(<?php echo $claim_id; ?>, true)' <?php } ?>>
                                            Edit Album
                                        </a>
                                    </div>
                                    <?php //} ?>
                                </td>
                            <?php
                            } //editable==true;
                            else {
                                ?>
                                <td width="100px">
                                    <div class="edit-album-btn clearfix">
                                        <a href="#"
                                           class="<?php echo $disabled_string; ?>" <?php if ($row['claim_status'] == 'out for review') print 'disabled'; ?> >
                                            Edit Album
                                        </a>
                                    </div>
                                </td>
                            <?php } ?>
                            <?php
                            if (!isset($user->roles[23])) {
                                ?>
                                <td width="100px">
                                    <a href="<?php echo base_path() . "account/additional_claim_info/$claim_id"; ?>"
                                       class="edit-additional-data-btn">Edit</a>
                                </td>
                            <?php
                            } else {
                                ?>
                                <td width="100px">
                                    <a href="#"
                                       class="edit-additional-data-btn">Edit</a>
                                </td>
                            <?php
                            }?>
                            <?php
                            if (!isset($user->roles[23])) {
                                ?>
                                <td>
                                    <!--<a href="#" class="create-invoice-btn enabled" onclick="open_submit_box(<?= $claim_id; ?>);return false;">Create My Invoice</a>-->
                                    <a class="create-invoice-btn enabled colorbox-node" href="<?php echo $base_url; ?>/account/confirm_submit_claim/<?= $claim_id; ?>?width=700&height=540">Create My Invoice</a>
                                </td>
                            <?php
                            } else {
                                ?>
                                <td><a href="#" class="create-invoice-btn enabled">Create My Invoice</a>
                                </td>
                            <?php
                            }?>
                        </tr>

                    <?php
                    } //end of for
                    for ($i = 0; $i < ($min_row - $row_count); $i++) {
                        print ('<tr>');
                        print ('<td colspan="4">&nbsp</td>');
                        print ('</tr>');
                    }
                    ?>
                </table>
            </div>
        </div>
        <div class="view-details clearfix">
            <a href="<?php echo $base_url ?>/account/my-active-claims.html">View Details</a>
        </div>
    </div>
<?php
}
//----------------------------------------------------------------------
function _ncn_claim_manager_homepage_out_for_reviews($min_row)
{

// param
    $order_by = "last_modified";
    $additional_sql = " AND (`claim_status`='out for review' OR `claim_status`='returned' OR `claim_status`='approved' ) ";
    $editable = true;
    $send_to_archive = false;
    $send_to_admin = false;
    $approve_deny = true;

    ?>

    <div class="ahome-section-wrapper">
        <div class="clearfix">
            <div class="title">Out for Review</div>
            <div class="help"><a href="#">Help</a></div>
        </div>
        <div class="table clearfix">
            <div class="table-header">
                <table>
                    <tr>
                        <td width="120px">Customer Name</td>
                        <td>Claim Status</td>
                    </tr>
                </table>
            </div>
            <div class="table-body">
                <table>
                    <?php
                    GLOBAL $user;
                    GLOBAL $base_url;

                    if ($editable == true) {
                        $_editable = 1;
                    } else {
                        $_editable = 0;
                    }

                    if ($send_to_admin == true) {
                        $_send_to_admin = 1;
                    } else {
                        $_send_to_admin = 0;
                    }

                    if ($approve_deny == true) {
                        $_approve_deny = 1;
                    } else {
                        $_approve_deny = 0;
                    }

                    if (isset($user->roles[23])) {
                        $query = "SELECT * FROM claims WHERE leaduser=" . $user->uid . " AND deleted=0 AND claim_status!='unpurchased' $additional_sql ORDER BY `$order_by` DESC limit 0, 4;;";
                    } else {
                        $query = "SELECT * FROM claims WHERE user_id=" . $user->uid . " AND deleted=0 AND claim_status!='unpurchased' $additional_sql ORDER BY `$order_by` DESC limit 0, 4;;";
                    }
                    $result = db_query($query);
                    $row_count = $result->rowCount();
                    
                    $orders_total = 0;
                    $payment_received_total = 0;

                    $result_rec = $result->fetchAll();
                    foreach($result_rec as $row)
                    {
                        // get data from sql
                        $row = (array)$row;
                        $claim_id = $row['claim_id'];

                        // does the file note exists/not blank
                        if ($row['file_note'] == '') // give the file notes some default value
                        {
                            $row['file_note'] = base64_encode('[no note]');
                        }


                        // does an invoice exist?
                      //  $query2 = "SELECT * FROM claims_invoices WHERE claim_id=" . $claim_id . " AND `live`=2;";
                        $result2 = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=2',
                        array(':a'=>$claim_id));
                        $live_invoice_count = $result2->rowCount();

                      //  $query2 = "SELECT * FROM claims_invoices WHERE claim_id=" . $claim_id . " AND `live`=1;";
                        $result2 = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=1',
                        array(':a'=>$claim_id));
                        $live_claim_count = $result2->rowCount();

                        // draw the row
                        ?>
                        <tr>
                            <td width="120px"><?php echo ncn_cd($claim_id, 'customer_name'); ?></td>
                            <td><?= ncn_claim_manager_claim_status_style($row['claim_status']); ?></td>
                        </tr>

                    <?php
                    } //end of for
                    for ($i = 0; $i < ($min_row - $row_count); $i++) {
                        print ('<tr>');
                        print ('<td colspan="2">&nbsp</td>');
                        print ('</tr>');
                    }
                    ?>
                </table>
            </div>
        </div>
        <div class="view-details clearfix">
            <a href="<?php echo $base_url ?>/account/out-for-review.html">View Details</a>
        </div>
    </div>
<?php
}

//----------------------------------------------------------------------
function _ncn_claim_manager_homepage_receivables($min_row) {

// param
    $order_by = "last_modified";
    $additional_sql = " AND claim_status= 'receivables' ";
    $editable = false;
    $send_to_archive = true;
    $send_to_admin = false;
    $approve_deny = false;

    ?>

<?php if ($GLOBALS['account_role'] == "leaduser"){ ?>
<div class="ahome-section-wrapper-lead">
<?php }else{ ?>
    <div class="ahome-section-wrapper">
        <?php } ?>
        <div class="clearfix">
            <div class="title">Receivables</div>
            <div class="help"><a href="#">Help</a></div>
        </div>
        <div class="table clearfix">
            <div class="table-header">
                <table>
                    <tr>
                        <td width="140px">Customer Name</td>
                        <td width="150px">Functions</td>
                        <td width="150px">Claim Amounts</td>
                        <td>File Notes</td>
                    </tr>
                </table>
            </div>
            <div class="table-body">
                <table>
                    <?php
                    GLOBAL $user;
                    GLOBAL $base_url;

                    if ($editable == true) {
                        $_editable = 1;
                    } else {
                        $_editable = 0;
                    }

                    if ($send_to_admin == true) {
                        $_send_to_admin = 1;
                    } else {
                        $_send_to_admin = 0;
                    }

                    if ($approve_deny == true) {
                        $_approve_deny = 1;
                    } else {
                        $_approve_deny = 0;
                    }

                    if (isset($user->roles[23])) {
                        $query = "SELECT * FROM claims WHERE leaduser=" . $user->uid . " AND deleted=0 AND claim_status!='unpurchased' $additional_sql ORDER BY `$order_by` DESC limit 0, 3;";
                    } else {
                        $query = "SELECT * FROM claims WHERE user_id=" . $user->uid . " AND deleted=0 AND claim_status!='unpurchased' $additional_sql ORDER BY `$order_by` DESC limit 0, 3;";
                    }
                    $result = db_query($query);
                    $row_count = $result->rowCount();

                    $orders_total = 0;
                    $payment_received_total = 0;

                    $result_rec = $result->fetchAll();                  
                    foreach($result_rec as $row)
                    {
                        // get data from sql
                        $row = (array)$row;
                        $claim_id = $row['claim_id'];

                        // does the file note exists/not blank
                        if ($row['file_note'] == '') // give the file notes some default value
                        {
                            $row['file_note'] = base64_encode('[no note]');
                        }


                        // does an invoice exist?
                      //  $query2 = "SELECT * FROM claims_invoices WHERE claim_id=" . $claim_id . " AND `live`=2;";
                        $result2 = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=2',
                        array(':a'=>$claim_id));
                        $live_invoice_count = $result2->rowCount();

                      //  $query2 = "SELECT * FROM claims_invoices WHERE claim_id=" . $claim_id . " AND `live`=1;";
                        $result2 = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=1',
                        array(':a'=>$claim_id));
                        $live_claim_count = $result2->rowCount();

                        // draw the row
                        ?>
                        <tr>
                            <td width="120px"><?php echo ncn_cd($claim_id, 'customer_name'); ?></td>
                            <td width="150px">
                                <?php    if ($send_to_archive == true && !isset($user->roles[23])) // payment received functions
                                {
                                    ?>
                                    <a href="#" class="pay-full"
                                       onclick="archive_claim('<?= $base_url; ?>', <?= $claim_id; ?>, '<?= $row['payment_received']; ?>');return false;">Pay
                                        in Full</a>
                                    <a href="#" class="pay-partial"
                                       onclick="partial_payment('<?= $base_url; ?>', <?= $claim_id; ?>);return false;">Partial
                                        Payment Received</a>
                                <?php }else if ($send_to_archive == true){
        ?>

                                <a href="#" class="pay-full">Paid In Full</a>
                                <a href="#" class="pay-partial">Partial Payment Received</a>

                            <?php } ?>
                            </td>
                            <td width="150px">
                                <?php    if ($row['claim_amount'] > 0) // only show claim amount if > 0
                                {
                                    //$orders_total += $row['claim_amount'];
                                    print '$' . number_format($row['claim_amount'], 2);
                                }
                                ?>
                            </td>
                            <td>
                                <!-- <a class="file-notes" href="#" onclick="set_file_note('<?= $base_url; ?>', <?= $claim_id; ?>, true);return false;">Set File Notes</a> -->
                                <!--<span class="file-notes">file-note</span>-->
                                <?php    if (!isset($user->roles[23])){ ?>
                                    <a class="file-notes" href="#" onclick="set_file_note('<?= $base_url; ?>', <?= $claim_id; ?>, true);return false;">Set File Notes</a>
                                <?php }else{ ?>
                                    <a class="file-notes" href="#">Set File Notes</a>
                                <?php } ?>
                            </td>
                        </tr>
                    <?php
                    } //end of for
                    for ($i = 0; $i < ($min_row - $row_count); $i++) {
                        print ('<tr>');
                        print ('    <td colspan="4">&nbsp</td>');
                        print ('</tr>');
                    }
                    ?>
                </table>
            </div>
        </div>
        <div class="view-details clearfix">
            <a href="<?php echo $base_url ?>/account/receivables.html">View Details</a>
        </div>
    </div>
<?php
}

function ncn_claim_manager_claim_status_style($claim_status)
{
    $str_status = ucwords($claim_status);
    return "<span class=\"claim_status $claim_status\">$str_status</span>";
}

function draw_sales_report_table($data)
{
    ?>
    <div class="clear-block">
        <div class="ncn-pager-ranger" style="font-weight: bold; margin-bottom:5px;"></div>
    </div>
    <div class="table-header">
        <table>
            <tr class="header">
                <td class="td-company-name"><strong>Company Name</strong></td>
                <td class="td-member-name"><strong>Member Name</strong></td>
                <td class="td-start-up-fee"><strong>Start-up Fee</strong></td>
                <td class="td-monthly-membership-fee"><strong>Monthly Membership Fee</strong></td>
                <td class="td-date"><strong>Date</strong></td>
                <td class="td-total-earned"><strong>Total Earned</strong></td>
            </tr>
        </table>
    </div>
    <div class="table-body">
        <?php
        drupal_add_css(drupal_get_path('module', 'ncn_message') . '/ncn_message.css', 'module', 'all', FALSE);
        drupal_add_js(drupal_get_path('module', 'ncn_message') . '/ncn_message.js');

        $page_records = 10;
        $row_count = count($data);
        $page_num = 0;

        $member_count = 0;
        foreach ($data as $key => $data_part) {
            if ($member_count % $page_records == 0) {
                $page_num++; ?>
                <table class="ncn-pager ncn-pager-page-<?php echo $page_num; ?>">
            <?php } ?>
            <tr>
                <td class="td-company-name"><?php echo $data_part[0]; ?></td>
                <td class="td-member-name"><?php echo $data_part[1]; ?></td>
                <td class="td-start-up-fee"><?php echo $data_part[2]; ?></td>
                <td class="td-monthly-membership-fee"><?php echo $data_part[3]; ?></td>
                <td class="td-date"><?php echo date('d M Y H:i:s', $data_part[4]); ?></td>
                <td class="td-total-earned"><?php echo render_payment_cost($data_part[5]); ?></td>
            </tr>
            <?php if ($member_count % $page_records == ($page_records - 1)): ?>
                </table>
            <?php endif; ?>
            <?php
            $member_count += 1;
        }
        if ($member_count == 0) {
            ?>
            <table class="">
                <?php
                for ($i = $member_count; $i < $page_records; $i++) {
                    ?>
                    <tr>
                        <td colspan="6">&nbsp;</td>
                    </tr>
                <?php } ?>
            </table>
        <?php
        } else if (($member_count % $page_records) != 0) {
            for ($i = $member_count; $i < (ceil($member_count / $page_records) * $page_records); $i++) {
                ?>
                <tr>
                    <td colspan="6">&nbsp;</td>
                </tr>
            <?php
            }
            echo "</table>";
        }
        ?>
    </div>
    <div class="ncn-pager-control clearfix">    <!-- pager_control -->
        <div style="float: right">
            <a class="first" href="#" onclick="pm_page_action('first');">first</a>
            <a class="prev" href="#" onclick="pm_page_action('prev');">prev</a>
            <a class="next" href="#" onclick="pm_page_action('next');">next</a>
            <a class="last" href="#" onclick="pm_page_action('last');">last</a>
        </div>
    </div>
    <script type="text/javascript">
        var total_records = <?php echo $member_count ?>;
        var page_records = <?php echo $page_records; ?>;
        var page_number = 1;    //global;
        var page_count = <?php echo $page_num; ?>;
        jQuery(document).ready(function () {
            pm_select_page(page_number);
            if (total_records <= page_records) {
                jQuery('.ncn-pager-ranger').css('display', 'none');
                jQuery('.ncn-pager-control').css('display', 'none');
            }
        });
    </script>
<?php
}


function ncn_claims_manager_draw_signup_fee_select($name, $default)
{
    // #options
    $payments = array();
    $pnode = node_load(SKU_SUF_DF);
    $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_DF] = $pnode; // NCN Membership Kit ($279)
    $pnode = node_load(SKU_SUF_DP);
    $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_DP] = $pnode; // NCN Membership Kit Options ($99)

    /* NCN GOLD MEMBER SPECIAL  */
    $pnode = node_load(SKU_SUF_GMSD);
    $pnode->data = unserialize($pnode->data);
    $payments[SKU_SUF_GMSD] = $pnode; // NCN Membership Kit Options ($49)

    /*$pnode = node_load(57);   $pnode->data = unserialize($pnode->data);
    $payments['57'] = $pnode;   // NCN Gold First Special($0)*/

    $select_field = "<select id=\"$name\" name=\"$name\"> \r\n";
    foreach ($payments as $key => $product) {
        $selected = ($key == $default) ? "selected" : "";
        $select_field .= "<option value=\"$key\" $selected >" . $product->title . " ($" . number_format($product->cost, 2) . ")</option>";
    }
    $select_field .= "</select> \r\n";

    return $select_field;
}

//------------------------------------------------------------------------------
function render_claim_time($time)
{

    $str = sprintf("%02d", $time['hour']) . ':' . sprintf("%02d", $time['minute']) . " " . $time['meridiem'];

    return $str;
}

//------------------------------------------------------------------------------
function render_claim_date($date)
{
    return date("F j Y", strtotime($date)); //return $date;

    if (!is_numeric($date)) {
        return '';
    }

    return date("m/d/Y", strtotime($date));
}
