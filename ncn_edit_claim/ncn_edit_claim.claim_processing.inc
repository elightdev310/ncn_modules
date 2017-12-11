<?php
function ncn_edit_claim_processing_page($claim_id) {
    $claim = get_claim_data($claim_id);
    
    ob_start();
    @include get_realpath('ncn_edit_claim', 'ui/claim_processing_page.php');
    $content = ob_get_contents();
    ob_end_clean();

    return $content;
}
