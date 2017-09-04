<?php
function ncn_capsulecrm_synchronize() {
	$max_cc_id = variable_get('MAX_CC_ID', 0);
	$result = db_query("SELECT * FROM {member_id_pool} WHERE id>:d ORDER BY id ASC",array(":d"=>$max_cc_id));
	if ($result->rowCount()>0) {
		$row=$result->fetchAssoc();
		ncn_capsulecrm_set_map_info( $row['member_id'] );
		$max_cc_id = $row['id'];
	} else {
		//$max_cc_id +=1;
	}
	variable_set('MAX_CC_ID', $max_cc_id);
	$str_ret = $max_cc_id;
	if (isset($row['member_id'])) { $str_ret.= "-" . $row['member_id']; }
	
	return $str_ret;
}

function ncn_capsulecrm_set_map_data($member_id, $org_id, $person_id) {
	$cc_info = ncn_capsulecrm_get_map_info($member_id);
	if ($cc_info) {
		db_query('UPDATE {ncn_capsulecrm_map} SET organization_id=:a, person_id=:b WHERE id=:c',
            array(':a'=>$org_id,':b'=>$person_id,':c'=>$cc_info['id']));
	} else {
		db_query('INSERT INTO {ncn_capsulecrm_map}(member_id, organization_id, person_id) VALUES(:a, :b, :c)',
            array(':a'=>$member_id,':b'=>$org_id,':c'=>$person_id));
	}
	return TRUE;
}

function ncn_capsulecrm_set_map_info($member_id) {
	$org_id = 0;
	$person_id = 0;
	if (ncn_capsulecrm_get_partyid_from_memberid($member_id, $org_id, $person_id)) {
		return ncn_capsulecrm_save_map_info($member_id, $org_id, $person_id);
	}
	return FALSE;
}

function ncn_capsulecrm_save_map_info($member_id, $org_id, $person_id) {
	$cc_info = ncn_capsulecrm_get_map_info($member_id);
	if ($cc_info) {
		if ($org_id!=$cc_info['organization_id'] || $person_id!=$cc_info['person_id']) {
			db_query("UPDATE {ncn_capsulecrm_map} SET organization_id=:a, person_id=:b WHERE id=:c",
                array(":a"=>$org_id,":b"=>$person_id,":c"=>$cc_info['id']));
		}
	} else {
		db_query("INSERT INTO {ncn_capsulecrm_map}(member_id, organization_id, person_id) VALUES(':a', ':b', ':c')",
            array(":a"=>$member_id,":b"=>$org_id,":c"=>$person_id));
	}
  
	return TRUE;
}

function ncn_capsulecrm_get_map_info($member_id) {

	$result = db_query('SELECT * FROM {ncn_capsulecrm_map} WHERE member_id=:s',array(':s'=>$member_id))->fetchAssoc();
//    $result = db_select('ncn_capsulecrm_map', 'n')
//        ->fields('n')
//        ->condition('member_id', $member_id,'=')
//        ->execute()
//        ->fetchAssoc();

    if ($result) {
		return $result;
	}
	return FALSE;
}
function ncn_capsulecrm_get_partyid_from_memberid($member_id, &$org_id, &$person_id) {	
	$query = "$member_id";
	$result = capsule_search_parties($query, 0, 100);
	if ($result->code==200) {
		$orgs = json_decode($result->data);
		if (!$orgs) { return FALSE; }
		$orgs = $orgs->parties->organisation;
		if ($orgs) {
			$member = get_member_from_id($member_id);
			if (is_array($orgs)) {
				foreach ($orgs as $org) {
					$party_id = $org->id;
					if (ncn_capsulecrm_check_match($member, $party_id)) {
						$org_id = $party_id;
						if ($people_party_id = ncn_capsulecrm_get_people_party_id($party_id)) {
							$person_id = $people_party_id;
						}
						return TRUE;
					}
				}
			} else {
				$org = $orgs;
				$party_id = $org->id;
				if (ncn_capsulecrm_check_match($member, $party_id)) {
					$org_id = $party_id;
					if ($people_party_id = ncn_capsulecrm_get_people_party_id($party_id)) {
						$person_id = $people_party_id;
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

function ncn_capsulecrm_check_match($member, $party_id) {
	$cc_member_id = ncn_capsulecrm_get_member_id($party_id);
	
	if (!$cc_member_id || $cc_member_id != $member['member_id']) {
		return FALSE;
	}
	return TRUE;
}

function ncn_capsulecrm_get_member_id($party_id) {
	$cc_custom_fields = ncn_capsulecrm_get_custom_fields($party_id);
	if ($cc_custom_fields) {
		$member_id = ncn_capsulecrm_get_custom_field_data($cc_custom_fields, 'Member ID #', 'text');
		if ($member_id) {
			return $member_id;
		}
	}
	return FALSE;
}

function ncn_capsulecrm_get_custom_fields($party_id) {
	$result = capsule_list_party_customfields($party_id);
	if ($result && $result->code == 200) {
		$data = json_decode($result->data);
		if (isset($data->customFields->customField)) {
			return $data->customFields->customField;
		}		
	}
	return FALSE;
}

function ncn_capsulecrm_get_custom_field_meta_data($field_data, $label) {
	if (is_array($field_data)) {
		foreach($field_data as $data) {
			if ($data->label == $label) {
				return $data;
			}
		}
	}
	return FALSE;
}

function ncn_capsulecrm_get_custom_field_data($field_data, $label, $type) {
	if (is_array($field_data)) {
		foreach($field_data as $data) {
			if ($data->label == $label) {
				return $data->$type;
			}
		}
	}
	return FALSE;
}

function ncn_capsulecrm_get_peoples($party_id) {
	$result = capsule_list_party_people($party_id);
	if ($result && $result->code == 200) {
		$peoples = json_decode($result->data);
		return $peoples->parties->person;
	}
	return FALSE;
}

function ncn_capsulecrm_get_people_party_id($party_id) {
	$person = ncn_capsulecrm_get_peoples($party_id);
	
	if ($person) {
		if (is_array($person)) {
			return $person[0]->id;
		} else {
			return $person->id;
		}
	}
	return FALSE;
}

function ncn_capsulecrm_get_url() {
	$key = variable_get('capsule_url_key', '');
    $key = ($key!='' ? $key : 'netclaimsnow-com');
	return "https://$key.capsulecrm.com/";

}

///////////////////////////////////////////////////////////////////////////////
function ncn_capsulecrm_daily_update_active_user_data() {
	$result = db_query("SELECT * FROM {users} WHERE uid!=0 ORDER BY uid DESC");
    foreach($result as $u_row)
    {
        $u_row = (array)$u_row;
		$user = user_load($u_row['uid']);
		if ($user) {
			if (isset($user->profile_memberid)&&is_member($user->profile_memberid)) {
				ncn_capsulecrm_daily_update_user_member_data($user);
			}
		} else {
		}
	}
}

function ncn_capsulecrm_daily_update_unactived_member_data() {
	$result = db_query("SELECT * FROM {member_id_pool} WHERE used=0");
//	while ($member = db_fetch_array($result))
    foreach($result as $member)
    {
        $member = (array)$member;
		$member_id = $member['member_id'];
		ncn_capsulecrm_daily_update_una_member_data($member_id);
	}
}

/**
 * Update			# of Claims , Total $$ YTD: , Total $$ All Time: 
 */
function ncn_capsulecrm_daily_update_user_member_data($user) {
	$crm_info = ncn_capsulecrm_check_map_info(isset($user->profile_memberid)?$user->profile_memberid:0);
	if (!$crm_info) {
		return FALSE;
	}
	
	$party_id = $crm_info['organization_id'];
	$cc_custom_fields = ncn_capsulecrm_get_custom_fields($party_id);
	
	// # of Claims
	$total_claims = user_detail_get_total_claims($user);
	if ($meta_data = ncn_capsulecrm_get_custom_field_meta_data($cc_custom_fields, '# of Claims')) {
		$data = array(
			'text'=>$total_claims, 
		);	
		$response = capsule_update_party_customfield($party_id, $meta_data->id, array('customField'=>$data));
	} else {
		$data = array(
			'label'=>'# of Claims', 
			'text'=>$total_claims, 
		);
		$response = capsule_add_party_customfield( $party_id, array('customField'=>$data) );
	}
	
	// Total $$ YTD:
	$ytd = render_payment_cost(ncn_report_get_ytd($user->uid));
	$ytd = strip_tags($ytd);
	if ($meta_data = ncn_capsulecrm_get_custom_field_meta_data($cc_custom_fields, 'Total $$ YTD:')) {
		$data = array(
			'text'=>$ytd, 
		);	
		$response = capsule_update_party_customfield($party_id, $meta_data->id, array('customField'=>$data));
	} else {
		$data = array(
			'label'=>'Total $$ YTD:', 
			'text'=>$ytd, 
		);
		$response = capsule_add_party_customfield( $party_id, array('customField'=>$data) );
	}
	
	// Total $$ All Time:
	$total_ytd = render_payment_cost(ncn_report_get_total_ytd($user->uid));
	$total_ytd = strip_tags($total_ytd);
	if ($meta_data = ncn_capsulecrm_get_custom_field_meta_data($cc_custom_fields, 'Total $$ All Time:')) {
		$data = array(
			'text'=>$total_ytd, 
		);	
		$response = capsule_update_party_customfield($party_id, $meta_data->id, array('customField'=>$data));
	} else {
		$data = array(
			'label'=>'Total $$ All Time:', 
			'text'=>$total_ytd, 
		);
		$response = capsule_add_party_customfield( $party_id, array('customField'=>$data) );
	}
	
}

/**
 * Update $YTD and $All Time for Unactivated Members
 */
function ncn_capsulecrm_daily_update_una_member_data($member_id) {
	$crm_info = ncn_capsulecrm_check_map_info($member_id);
	if (!$crm_info) {
		return FALSE;
	}
	
	$party_id = $crm_info['organization_id'];
	$cc_custom_fields = ncn_capsulecrm_get_custom_fields($party_id);
	
	// Total $$ YTD:
	$ytd = render_payment_cost(ncn_report_get_ytd(0, $member_id));
	$ytd = strip_tags($ytd);
	if ($meta_data = ncn_capsulecrm_get_custom_field_meta_data($cc_custom_fields, 'Total $$ YTD:')) {
		$data = array(
			'text'=>$ytd, 
		);	
		$response = capsule_update_party_customfield($party_id, $meta_data->id, array('customField'=>$data));
	} else {
		$data = array(
			'label'=>'Total $$ YTD:', 
			'text'=>$ytd, 
		);
		$response = capsule_add_party_customfield( $party_id, array('customField'=>$data) );
	}
	
	// Total $$ All Time:
	$total_ytd = render_payment_cost(ncn_report_get_total_ytd(0, $member_id));
	$total_ytd = strip_tags($total_ytd);
	if ($meta_data = ncn_capsulecrm_get_custom_field_meta_data($cc_custom_fields, 'Total $$ All Time:')) {
		$data = array(
			'text'=>$total_ytd, 
		);	
		$response = capsule_update_party_customfield($party_id, $meta_data->id, array('customField'=>$data));
	} else {
		$data = array(
			'label'=>'Total $$ All Time:', 
			'text'=>$total_ytd, 
		);
		$response = capsule_add_party_customfield( $party_id, array('customField'=>$data) );
	}
	
}

function ncn_capsulecrm_check_map_info($member_id) {
	$crm_info = ncn_capsulecrm_get_map_info($member_id);
	if (!$crm_info) {
		if (ncn_capsulecrm_set_map_info($member_id)) {
			$crm_info = ncn_capsulecrm_get_map_info($member_id);
		}
	}
	return $crm_info;
}

function ncn_capsulecrm_add_note($member_id, $note) {
	$crm_info = ncn_capsulecrm_check_map_info($member_id);
	if (!$crm_info) {
		return FALSE;
	}
	
	$party_id = $crm_info['organization_id'];
	$data = array(
		'note'=> $note, 
	);
	$response = capsule_add_party_history($party_id, array('historyItem'=>$data));
	
	if ($response->code != 201)  {
		drupal_set_message("Failed to add a note to capsule crm.", 'error');
		return FALSE;
	}
	return TRUE;
}

function ncn_capsulecrm_add_task($member_id, $note) {
}

/**
 * Set VOS Account field ( user detail page )
 */
function ncn_capsulecrm_set_user_detail_link($user_id, $member_id=0) {
	if ($member_id == 0) {
		$_user = user_load($user_id);
		if ($_user && isset($_user->profile_memberid) && $_user->profile_memberid) {
			$member_id = $_user->profile_memberid;
		} else {
			return FALSE;
		}
	}
	
	$cc_info = ncn_capsulecrm_get_map_info($member_id);
	if (!$cc_info || !$cc_info['organization_id']) {
		if (ncn_capsulecrm_set_map_info($member_id)) {
			$cc_info = ncn_capsulecrm_get_map_info($member_id);
		} else {
			return FALSE;
		}
	}
	
	$party_id = $cc_info['organization_id'];
  
	if (!$party_id) { return FALSE; }
  
	$cc_custom_fields = ncn_capsulecrm_get_custom_fields($party_id);
  $user_url = "http://www.netclaimsnow.com/admin/user/user/detail/".$user_id;
  
  if ($meta_data = ncn_capsulecrm_get_custom_field_meta_data($cc_custom_fields, 'VOS account')) {
    $data = array(
      'text'=>$user_url, 
    );  
    $response = capsule_update_party_customfield($party_id, $meta_data->id, array('customField'=>$data));
  } else {
	// Custom Field (VOS account)
	  $data = array(
		  'label'=>'VOS account', 
		  'text'=>$user_url
	  );
	  $response = capsule_add_party_customfield( $party_id, array('customField'=>$data) );
  }
	if ($response->code != 201)  {
		//drupal_set_message(t('Capsule CRM Error: Failed to add a customfield (!label : !text)', array('!label'=>$data['label'], '!text'=>$data['text'])), 'error');
		return FALSE;
	}
	return TRUE;
}
/**
 * Synchronize VOS Account fields ( Maintain ) 
 */
 
function ncn_capsulecrm_set_user_detail_link_maintain() {
	$result = db_query("SELECT * FROM {users} WHERE uid!=0 ORDER BY uid DESC");
//	while ($row = db_fetch_array($result))
    foreach($result as $row)
    {
        $row = (array)$row;
		$uid = $row['uid'];
		$_user = user_load($uid);
		if (isset($_user->profile_memberid) && $_user->profile_memberid) {
			ncn_capsulecrm_set_user_detail_link($uid);
		}
	}
}
