<form method="POST" id="term-agree-form">
  <input type="hidden" name="tfunction" value="process">
  <input type="hidden" name="page" value="2">
  <div class="term-conditions"><tt>
    <p align="justify">
      <?php 
			if ($member_type == 1) { echo nl2br(variable_get('ncn_terms_text_silver', '')); } 
			else if ($member_type == 2) { echo nl2br(variable_get('ncn_terms_text_gold_lite', '')); } 
			else if ($member_type == 3) { echo nl2br(variable_get('ncn_terms_text_coach_on_call', '')); } 
      else { echo nl2br(variable_get('ncn_terms_text', '')); } 
		?>
    </p>
    </tt></div>
  <br />
  <div class="create-claim-agree-div"> <a class="create-claim-agree-btn" href="#" onclick="create_claim_agree()">I Agree</a> <span>&nbsp; or &nbsp;<a href="<?= $GLOBALS['base_url']; ?>/account">cancel</a></span> </div>
</form>
