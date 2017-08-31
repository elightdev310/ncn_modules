<?php

/**
* Add note to party in CapsuleCRM
* 
* @param mixed $claim_id
* @param mixed $note
*/
function ncn_capsulecrm_note_add_note($claim_id, $note) {
global $user;

  $claim = get_claim_data($claim_id);
  if ($claim && $claim['user_id'] && $account=user_load($claim['user_id'])) {
    $member_id = $account->profile_memberid;
    if ($member_id && $cc_info=ncn_capsulecrm_get_map_info($member_id)) {
      if ($cc_info['organization_id']) {
        $data = array(
          'note'=>"[claim #$claim_id] - ".$note, 
        );
        $capsulecrm_account = ncn_capsulecrm_account_get_account($user);
        if ($capsulecrm_account) {
          $data['creator'] = $capsulecrm_account;
        }
        
        $response = capsule_add_party_history($cc_info['organization_id'], array('historyItem'=>$data) );
        if ($response->code != 201) { 
          drupal_set_message(t('Capsule CRM Error: Failed to add an NOTE into Capsule CRM'), 'error');
          return 0; 
        }
        $history_id = substr($response->headers['Location'], strrpos($response->headers['Location'], '/')+1);
        if (is_numeric($history_id)) {
          return $history_id;
        }
      }
    }
  }
  return 0;
}

/**
* Update note to party in CapsuleCRM
* 
* @param mixed $history_id
* @param mixed $note
*/
function ncn_capsulecrm_note_update_note($claim_id, $history_id, $note) {
global $user;
  $data = array('note'=>"[claim #$claim_id] - ".$note);
  $capsulecrm_account = ncn_capsulecrm_account_get_account($user);
  if ($capsulecrm_account) {
    $data['creator'] = $capsulecrm_account;
  }
        
  $response = capsule_update_history($history_id, array('historyItem'=>$data) );
  if ($response->code != 200) { 
    drupal_set_message(t('Capsule CRM Error: Failed to update NOTE into Capsule CRM'), 'error');
    return FALSE; 
  }
  
  return TRUE;
}