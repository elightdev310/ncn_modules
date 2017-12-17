<div id="page_loading"></div>
<div id="page_results" class="clearfix">
    <?php print ncn_member_logo_legalname_block($user->uid); ?>
    <!-- 3 Box -->
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

    <!--  -->
    <div class="dashboard-claim-section">
        <ul class="nav nav-pills">
            <li class="active"><a data-toggle="pill" href="#d_claims">Claims</a></li>
            <li><a data-toggle="pill" href="#d_invoices">Invoices</a></li>
            <li><a data-toggle="pill" href="#d_more_info">More Info Needed</a></li>
            <li><a data-toggle="pill" href="#d_invoices_completed">Invoices Completed</a></li>
            <li><a data-toggle="pill" href="#d_out_approval">Out for Approval</a></li>
            <li><a data-toggle="pill" href="#d_ars">ARS in Progress</a></li>
            <li><a data-toggle="pill" href="#d_more">More</a></li>
        </ul>

        <div class="tab-content panel-box">
            <div id="d_claims" class="tab-pane fade in active panel-box-content">
                <div class="ncn-data-table table-responsive">
                    <table class="table">
                    <thead>
                        <tr>
                            <th class="td-id">ID</th>
                            <th class="td-created-at">Created Date</th>                            
                            <th class="td-insured-name">Insured Name</th>
                            <th class="td-photo-album">Photo Album</th>
                            <th class="td-property-loss-address">Property Loss Address</th>
                            <th class="td-rooms">Rooms</th>
                            <th class="td-claim-processing">Claim Processing</th>
                            <th class="td-create-my-invoice">Create My Invoice</th>
                        </tr>
                    </thead>
                    <tbody>
                    <?php if (empty($claims)): ?>
                        <tr>
                            <td colspan="7">No claim</td>
                        </tr>
                    <?php else: ?>
                        <?php foreach($claims as $row): ?>
                        <?php 
                            $row = (array)$row;
                            $editable = true;
                            $send_to_archive = false;
                            $send_to_admin = false;
                            $approve_deny = false;
                            switch ($row['claim_status']) {
                                case 'incomplete':
                                    $editable = true;
                                    $send_to_archive = false;
                                    $send_to_admin = true;
                                    break;
                                case 'out for review':
                                case 'returned':
                                case 'approved':
                                    $editable = true;
                                    $send_to_archive = false;
                                    $send_to_admin = false;
                                    $approve_deny = true;
                                    break;
                                case 'archived': 
                                    $editable = false;
                                    break;
                            }
                        ?>
                            <tr>
                                <td class="td-id">
                                    <?php echo $row['claim_id'] ?>
                                </td>
                                <td class="td-created-at">
                                    <?php echo strTime($row['created']) ?>
                                </td>
                                <td class="td-insured-name">
                                    <?php echo ncn_cd($row['claim_id'], 'customer_name') ?>
                                </td>
                                <td class="td-photo-album">
                                    <?php if ($editable == true): ?>
                                    <a class="enabled btn btn-primary" onclick='open_edit_box(<?php echo $row['claim_id']; ?>)'>
                                        <span class="btn-camera-icon">Edit Album</span>
                                    </a>
                                    <?php endif; ?>
                                </td>
                                <td class="td-property-loss-address">
                                    <?php echo strClaimAddress($row['claim_id']) ?>
                                </td>
                                <td class="td-rooms">
                                    <?php if ($editable == true): ?>
                                    <a class="btn btn-primary" onclick="open_scopesheet_edit_box(<?php echo $row['claim_id']; ?>)">Edit</a>
                                    <?php endif; ?>
                                </td>
                                <td class="td-claim-processing">
                                    <?php if ($send_to_admin == true && !is_leaduser($user)): ?>
                                    <a href="<?php print url("account/edit_claim/".$row['claim_id']) ?>" class="btn btn-primary">Edit</a>
                                    <?php endif; ?>
                                </td>
                                <td class="td-create-my-invoice">
                                    <?php if ($send_to_admin == true && !is_leaduser($user)): ?>
                                    <a class="create-invoice-btn enabled colorbox-node btn btn-primary" href="<?php echo $base_url; ?>/account/confirm_submit_claim/<?= $claim_id; ?>?width=700&height=540">Create My Invoice</a>
                                    <?php endif; ?>
                                    
                                </td>
                            </tr>
                        <?php endforeach; ?>
                    <?php endif; ?>
                    </tbody>
                    </table>
                </div>
            </div>
            <div id="d_invoices" class="tab-pane fade panel-box-content">
              <h3>Invoices</h3>
              <p>Coming Soon...</p>
            </div>
            <div id="d_more_info" class="tab-pane fade panel-box-content">
              <h3>More Info Needed</h3>
              <p>Coming Soon...</p>
            </div>
            <div id="d_invoices_completed" class="tab-pane fade panel-box-content">
              <h3>Invoices Completed</h3>
              <p>Coming Soon...</p>
            </div>
            <div id="d_out_approval" class="tab-pane fade panel-box-content">
              <h3>Out for Approval</h3>
              <p>Coming Soon...</p>
            </div>
            <div id="d_ars" class="tab-pane fade panel-box-content">
              <h3>ARS in Progress</h3>
              <p>Coming Soon...</p>
            </div>
            <div id="d_more" class="tab-pane fade panel-box-content">
              <h3>More</h3>
              <p>Coming Soon...</p>
            </div>
        </div>
    </div>

    <!-- Chatter -->
    <?php if (module_exists('ncn_chatter')): ?>
    <div class="chatter-section">
        <div class="chatter-section-content">
            <?php print ncn_chatter_user_news_feed_block() ?>
        </div>
    </div>
    <?php endif; ?>
</div>
