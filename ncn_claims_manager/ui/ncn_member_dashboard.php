<div id="page_loading"></div>
<div id="page_results" class="clearfix">
    <div class="row dashboard-claim-summary">
        <div class="col-sm-4">
            <div class="panel-box">
                <div class="panel-box-content">
                    <div class="box-title">Average Invoice Amount</div>
                    <div class="box-value"><?php print '$' . number_format($average_claim_value, 2); ?></div>
                </div>
            </div>
        </div>
        <div class="col-sm-4">
            <div class="panel-box">
                <div class="panel-box-content">
                    <div class="box-title">Number of MYNCN Claims Lifetime</div>
                    <div class="box-value"><?php print $total_claims; ?></div>
                </div>
            </div>
        </div>
        <div class="col-sm-4">
            <div class="panel-box">
                <div class="panel-box-content">
                    <div class="box-title">Total MYNCN Claim Value</div>
                    <div class="box-value"><?php print '$' . number_format($total_claim_value, 2); ?></div>
                </div>
            </div>
        </div>
    </div>

    <?php print ncn_member_logo_legalname_block($user->uid); ?>

    <?php if (module_exists('ncn_chatter')): ?>
    <div class="chatter-section">
        <div class="chatter-section-content">
            <?php print ncn_chatter_user_news_feed_block() ?>
        </div>
    </div>
    <?php endif; ?>
</div>
