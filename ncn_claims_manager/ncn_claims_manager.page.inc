<?php

/**
 * NCN Member - Dashboard
 */
function ncn_claim_manager_member_dashboard_page()
{
global $user;
    $total_claim_value = invoiced_to_date($user->uid, true);
    $total_claims = user_detail_get_total_claims($user);
    $average_claim_value= $total_claim_value / $total_claims;

    //
    $claims = db_select('claims', 'c')
            ->fields('c')
            ->condition('c.user_id', $user->uid)
            ->condition('c.deleted', 0)
            ->condition('c.claim_status', 'incomplete')
            ->orderBy('c.last_modified', 'DESC')
            ->extend('PagerDefault')
            ->limit(5)
            ->execute()
            ->fetchAll();

    ob_start();
    @include get_realpath('ncn_claims_manager', 'ui/ncn_member_dashboard.php');
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
}

/**
 * Claims Page (NCN Member)
 * URL: account/claims
 */
function ncn_claims_manager_claims_list_page() {
global $user;
    
    $filter = isset($_REQUEST['filter'])?$_REQUEST['filter']:'all';
    $db_or = db_or();
    if ($filter == 'all') {
        $db_or->condition('1', 1);
    } else {
        $db_or->condition('c.claim_status', $filter);
    }
    // if ($filter == 'active') {
    //     $db_or->condition('c.claim_status', 'incomplete');
    //     $class_name = 'my-active-claims-detail';
    // } else if( $filter == 'out_for_review' || 
    //            $filter == 'returned' || 
    //            $filter == 'approved' ) {
    //     $db_or->condition('c.claim_status', 'out for review');
    //     $db_or->condition('c.claim_status', 'returned');
    //     $db_or->condition('c.claim_status', 'approved');
    //     $class_name = 'out-for-review-detail';
    // } else if ($filter == 'archived') {
    //     $db_or->condition('c.claim_status', 'archived');
    //     $class_name = 'archived-claims-detail';
    // } else {
    //     $db_or->condition('c.claim_status', 'incomplete');
    //     $db_or->condition('c.claim_status', 'out for review');
    //     $db_or->condition('c.claim_status', 'returned');
    //     $db_or->condition('c.claim_status', 'approved');
    //     $db_or->condition('c.claim_status', 'archived');
    //     $class_name = 'all-claims-detail';
    // }

    $per_page = 10;

    $claims = db_select('claims', 'c')
            ->fields('c')
            ->condition('c.user_id', $user->uid)
            ->condition('c.deleted', 0)
            ->condition('c.claim_status', 'unpurchased', '!=')
            ->condition($db_or)
            ->orderBy('c.last_modified', 'DESC')
            ->extend('PagerDefault')
            ->limit($per_page)
            ->execute()
            ->fetchAll();

    ob_start();
    @include get_realpath('ncn_claims_manager', 'ui/claims_list.php');
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
}


/**
 * ARS Page (NCN Member)
 * URL: account/ars.html
 */
function ncn_claims_manager_ars_list_page() {
global $user;
    
    $filter = isset($_REQUEST['filter'])?$_REQUEST['filter']:'all';
    $db_or = db_or();
    if ($filter == 'all') {
        $db_or->condition('1', 1);
    } else {
        $db_or->condition('c.claim_status', $filter);
    }

    $per_page = 10;

    $query = db_select('claims', 'c');
    $query->leftJoin('claims_data_extra', 'cde', 
                      'cde.claim_id = c.claim_id AND cde.name=:cde_name', 
                      array(':cde_name'=>'claim_ars'));
    $claims= $query->fields('c')
            ->condition('c.user_id', $user->uid)
            ->condition('c.deleted', 0)
            ->condition('c.claim_status', 'unpurchased', '!=')
            ->condition('cde.val', 1)   // ARS
            ->condition($db_or)
            ->orderBy('c.last_modified', 'DESC')
            ->extend('PagerDefault')
            ->limit($per_page)
            ->execute()
            ->fetchAll();

    $class_name = 'recievables-detail';
    ob_start();
    @include get_realpath('ncn_claims_manager', 'ui/ars_list.php');
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
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
