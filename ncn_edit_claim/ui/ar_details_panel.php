<?php
/**
 * AR Details Panel
 */
?>
<div class="ar-details-panel">
    <div class="sub-section">
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Account Manager<a class="about-icon" href="#">about</a></label>
                <div class="form-value">Yuka Nougchi</div>
            </div>
            <div class="form-group col-sm-6">
                <label>ARS Number<a class="about-icon" href="#">about</a></label>
                <div class="form-value"><?php print $claim_id; ?></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Claim Number<a class="about-icon" href="#">about</a></label>
                <div class="form-value"><?php print $claim_id; ?></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Member Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"><?php echo user_fullname($member_user); ?></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>File Status<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Primary Contact<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-12">
                <label>File Last Updated<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-12">
                <label>NCN Comparative<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-12">
                <label>Undisputed Amount<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Negotiation Details</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Amount Agreed Upon by Member<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Difference Original vs Agreed<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Claimant Information</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Claimant First Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Claimant Last Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Property Information</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Property Address<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Property City<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Property State<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Property Zip Code<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Carrier Information</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Carrier Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Carrier Phone Number<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Carrier Policy Number<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Carrier Fax Number<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Carrier Claim Number<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Carrier Email Address<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Adjuster Information</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Adjuster First Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Adjuster Last Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Secondary Adjuster</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Secondary Adjuster First Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Secondary Adjuster Last Name<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Peer Review</h4>
        <div class="form-group">
            <label>Peer Review Company<a class="about-icon" href="#">about</a></label>
            <div class="form-value"></div>
        </div>
        <div class="form-group">
            <label>Peer Review  Contact First Name<a class="about-icon" href="#">about</a></label>
            <div class="form-value"></div>
        </div>
        <div class="form-group">
            <label>Peer Review Contact Last Name<a class="about-icon" href="#">about</a></label>
            <div class="form-value"></div>
        </div>
    </div>

    <div class="sub-section">
        <h4>HOA Information</h4>
        <div class="form-group">
            <label>HOA Name<a class="about-icon" href="#">about</a></label>
            <div class="form-value"></div>
        </div>
        <div class="form-group">
            <label>HOA Contact First Name<a class="about-icon" href="#">about</a></label>
            <div class="form-value"></div>
        </div>
        <div class="form-group">
            <label>HOA Contact Last Name<a class="about-icon" href="#">about</a></label>
            <div class="form-value"></div>
        </div>
    </div>

    <div class="sub-section">
        <h4>Important Dates</h4>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Date File Open<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Date Submitted to Carrier<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Date Sent to Client for Review<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Date Carrier Confirmed Receipt<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-6">
                <label>Date Approved for Submission<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
            <div class="form-group col-sm-6">
                <label>Date Negotiation Completed<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
        <div class="row">
            <div class="form-group col-sm-12">
                <label>Date File Closed<a class="about-icon" href="#">about</a></label>
                <div class="form-value"></div>
            </div>
        </div>
    </div>

</div>
