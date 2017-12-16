<?php
/**
 * Room ScopeSheet - Claim Info Panel
 *
 * @var
 *     $claim_id
 */
$photo_album_url = url("account/edit_claim_inline/$claim_id/room/".$room_name);
?>
<div class="aci-claim-info-wrapper ss-claim-info-wrapper clearfix">
    <div class='aci-claim-id aci-claim-info-item ss-claim-info-item'>
        Claim ID: <?php print $claim_id; ?>
    </div>
    <div class='aci-claim-room aci-claim-info-item ss-claim-info-item'>
        Room Name: <?php print ncn_claims_manager_scopesheet_render_select_room_name($claim_id, "ncn_cm_ss_room_name", $room_name, $mode) ?>
    </div>
    <div class='aci-claim-add-room aci-claim-info-item ss-claim-info-item'>
        <a href="#" class="add-room-link">Add Room</a>
    </div>

    <div class='aci-claim-info-item ss-claim-info-item align-right'>
    <?php if ($mode=='edit'): ?>
        <?php $view_url = url("account/scope_sheet_view/$claim_id/$room_name"); ?>
        <a href="<?php print $view_url; ?>"><b>VIEW</b> Scopesheet</a>
    <?php else: ?>
        <?php $edit_url = url("account/scope_sheet/$claim_id/$room_name"); ?>
        <a href="<?php print $edit_url; ?>"><b>EDIT</b> Scopesheet</a>
    <?php endif; ?>

        &nbsp;&nbsp;|&nbsp;&nbsp;
        <a href="<?php print $photo_album_url; ?>"><b>Edit</b> Photo Album</a>
    </div>
</div>

<script>
jQuery(document).ready(function() {
    jQuery('#ncn_cm_ss_room_name').bind('change', function() {
        var url = jQuery('#ncn_cm_ss_room_name').val();
        window.location = url;
    });
});
</script>
