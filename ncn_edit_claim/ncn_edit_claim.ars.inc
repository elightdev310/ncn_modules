<?php
/**
 * URL: account/ar/%
 */
function ncn_edit_claim_ar_page($claim_id) {
    $claim = get_claim_data($claim_id);
    $member_user = user_load($claim['user_id']);
    $aci_icp = ncn_admin_get_aci_insurance_claim_policy($claim_id);
    
    ob_start();
    @include get_realpath('ncn_edit_claim', 'ui/ar_page.php');
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
}

function ncn_edit_claim_ar_details_panel($claim_id) {
    $claim = get_claim_data($claim_id);
    $member_user = user_load($claim['user_id']);

    $aci_poi = ncn_admin_get_aci_property_owner_info($claim_id);
    $aci_icp = ncn_admin_get_aci_insurance_claim_policy($claim_id);
    
    ob_start();
    @include get_realpath('ncn_edit_claim', 'ui/ar_details_panel.php');
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
}
