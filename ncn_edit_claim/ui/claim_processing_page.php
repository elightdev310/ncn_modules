<?php
/**
 * 
 */
?>

<div id="page_loading"></div>
<div id="page_results" class="claim-processing-page">
    <div class="claim-icon-section">
        <div>Claim</div>
        <div class="name-info"><?php echo ncn_cd($claim_id, 'customer_name') ?></div>
    </div>

    <div class="panel-box">
        <div class="panel-box-content">
            <div class="ncn-data-table table-responsive">
            <table class="table">
            <thead>
                <tr>
                    <th class="td-id">NCN Number</th>
                    <th class="td-claim-status">Claim Status</th>
                    <th class="td-property-loss-address">Property Loss Address</th>
                    <th class="td-claim-type">Claim Type</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td class="td-id">
                        <?php echo $claim_id ?>
                    </td>
                    <td class="td-claim-status">
                        <?php echo ncn_claim_manager_claim_status_style($claim['claim_status']) ?>
                    </td>
                    <td class="td-property-loss-address">
                        <?php echo strClaimAddress($claim_id) ?>
                    </td>
                    <td class="td-claim-type">
                        <?php print ucwords($claim['claim_type']) ?>
                    </td>
                </tr>
            </tbody>
            </table>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="claims-detail col-sm-8 col-md-7"> 
            <div class="claims-section-wrapper">
                <div class="aci-section-links"><?php print ncn_claims_manager_aci_render_section_links($claim_id); ?></div>
                <div class="aci-section-content-wrapper"><?php print ncn_claims_manager_aci_render_section_content($claim_id); ?></div>
            </div>
        </div>
        <div class="col-sm-4 col-md-5"> 
            <?php print ncn_chatter_claim_feed_block($claim_id); ?>
        </div>
    </div>

</div>
