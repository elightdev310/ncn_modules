<?php

function payment_transaction_page() {
GLOBAL $base_url;
  if (arg(4) == 'add') {
    return payment_transaction_add_page();
  }
  
  if (arg(4) == 'edit') {
    return payment_transaction_edit_page(arg(5));
  }
  
  
  if (isset($_REQUEST['tfunction']) && ($_REQUEST['tfunction'] == "refund_transaction")) {
    ncn_admin_refund_payment(intval($_REQUEST['param_pid']));
  } else if(isset($_REQUEST['tfunction']) && ($_REQUEST['tfunction'] == "delete_transaction")) {
    delete_payment_transaction(intval($_REQUEST['param_pid']));
  }
  drupal_add_js(drupal_get_path('module', 'ncn_admin') . '/ncn_admin.js');
  
  $filter = 'all';
  if (arg(4) == 'filter') {
    $filter = arg(5);
  }
  $query = "SELECT payment_log.*, claims.expedite FROM payment_log LEFT JOIN claims ON (payment_log.claim_id=claims.claim_id) WHERE status=1 ";

  if ( $filter=='all') {
    $where='';
  } else {
    $where="AND product_id=".$filter;
  }
  
  $specific_date = (isset($_REQUEST['specific_date']) ? $_REQUEST['specific_date'] : false);
  $is_expedite = (isset($_REQUEST['claim_expedite']) ? $_REQUEST['claim_expedite'] : false);
  if (!$is_expedite) { $is_expedite = 'all'; }
  
  if ($specific_date) {
    //$val_specific_date = date('m/d/Y', $specific_date);
    $from_t = strtotime($specific_date);
    $to_t = strtotime('+1 day', $from_t);
    $where .= " AND timestamp>=$from_t AND timestamp<$to_t";
  }
  
  if ($is_expedite != 'all') {
    $b_expedite = 0;
    if ($is_expedite == 'expedite') {
      $b_expedite = 1;
    }
    $where .= " AND expedite=$b_expedite";
  }

  $page = 0;
  if (isset($_REQUEST['page'])) {
    $page = intval($_REQUEST['page']);
  }  
  $query .= $where;
  
  $tot_result = db_query("SELECT COUNT(*) as tot FROM payment_log LEFT JOIN claims ON (payment_log.claim_id=claims.claim_id) WHERE status=1 ".$where)->fetchAssoc();
  $total_count = intval($tot_result['tot']);  
  $num_per_page = 50;
  if ($total_count!=0 && $page >= ceil($total_count/$num_per_page)) {
    $page = ceil($total_count/$num_per_page)-1;
  }
  $query .= " ORDER BY timestamp DESC";
  $query .= " LIMIT ".($page*$num_per_page).", $num_per_page";
  $result = db_query($query);
  $row_count = 0;
  if ($result) {
    $row_count = $result->rowCount();
  }
  
  $data_type = get_transaction_type('all');
  
  $table_head = '
  <thead class="tableHeader-processed">
    <th>PID</th>
    <th>Date</th>
    <th>Type</th>
    <th>Name</th>
    <th>Cost</th>
    <th colspan="3">Operation</th>
  </thead>
  <tbody>';

  $table_foot = '
  </tbody>
  </table>';
  
  $url = $base_url.'/admin/config/ncn_admin/transaction';
  
  drupal_add_js("
    jQuery(function() {
    jQuery( '#specific_date' ).datepicker({
      showOn: \"button\",
      buttonImage: \"$base_url/".drupal_get_path('module', 'ncn_report')."/images/calendar.gif\",
      buttonImageOnly: true,
      dateFormat: 'mm/dd/yy',
    });
  });", 'inline');
  
  ob_start();
?>
<div id="payment_transaction_page">
  <div class="description">  </div>
  <form id="transaction_filter" method="POST">
  <div class="transaction-where">
    <fieldset>
      <legend>Show only transactions where</legend>
      <div class="clear-block">
      <dl class="where">
        <dd class="a">
          <div>Transaction Type</div>
          <div>Expedite of Claim</div>
          <div>Date</div>
        </dd>
        <dt>is</dt>
        <dd class="b">
          <div>
            <?php echo draw_select_transaction_type('filter', 'payment_filter', $filter, 'class=" "'); ?>
          </div>
          <div>
            <?php echo draw_select_filter_expedite('claim_expedite', $is_expedite, 'class=" "'); ?>
          </div>
          <div>
            <input type="text" id="specific_date" name="specific_date" value="<?php echo $specific_date; ?>" />
          </div>
        </dd>
      </dl>
      </div>
      <div class="filter-panel">
        <input type="hidden" name="tfunction" id="param_tfunction" value="" />
        <input type="hidden" name="param_pid" id="param_pid" value="" />
        <dl><input id="edit-submit" class="form-submit" type="button" value="  Filter  " onclick="jump_filter_url('<?php echo $url; ?>');"></dl>
      </div>
    </fieldset>
  </div>
  </form>
  
  <fieldset>
    <legend>Report</legend>
    <div class="transaction-panel report-panel">
      <a href="<?php echo $base_url; ?>/download/transaction">Download Transaction List</a>&nbsp;&nbsp;&nbsp;
    </div>
  </fieldset>
  
  <fieldset>
    <legend>Add Transaction</legend>
    <div class="transaction-panel">
      <a href="<?php echo $base_url; ?>/admin/config/ncn_admin/transaction/add/standard">Add a standard transaction</a>&nbsp;&nbsp;&nbsp;
      <a href="<?php echo $base_url; ?>/admin/config/ncn_admin/transaction/add/additional">Add an additional transaction</a>
    </div>
  </fieldset>
  
  <div class="transaction-table">
<?php 
  // no results message
  if ($total_count == 0)
  {
    print "<table>";
    print $table_head;
    print $table_foot;
  } else {
    echo '<table class="sticky-enabled tableSelect-processed sticky-table">';
    echo $table_head;
    
    //for($i=0; $i<$row_count; $i++) 
	foreach($result as $row)
	{
      $row = (array)$row; 
      $ptype = get_item_from_key($data_type, 'ptype', $row['ptype']);
      if ($row['claim_id']!=0 && ncn_cd($row['claim_id'], 'expedite')) { $ptype['name'].=" - EXPEDITE"; }
      $pname = get_payment_from($row);
      
      $str_cost = render_payment_cost($row['cost']);
        
      // draw row
      ?>
      <tr class="<?php if ($i%2) print "even"; else print "odd"; ?> <?php if($row['refund']==-1) print "refund"; ?>">
        <td><?php echo $row['pid']; ?></td>
        <td><?php echo date('d M, Y H:i', $row['timestamp']); ?></td>
        <td><?php echo $ptype['name']; ?></td>
        <td><?php echo $pname; ?></td>
        <td><?php echo $str_cost; ?></td>
        <td><a href="<?php echo $base_url; ?>/admin/config/ncn_admin/transaction/edit/<?php echo $row['pid']; ?>">Edit</a></td>
        <td><?php if($row['refund']==0): ?>
          <a href="#" onclick="on_click_transaction_refund(<?php echo $row['pid']; ?>)">Refund</a>
          <?php elseif($row['refund']==1): ?>
          <span style="color: #808080;">Refunded</span>
          <?php endif; ?>
        </td>
        <td><a href="#" onclick="on_click_transaction_delete(<?php echo $row['pid']; ?>)">Delete</a></td>
        
      </tr>
      <?php
    }
    
    echo $table_foot;
    $url = $base_url."/".drupal_get_path_alias($_GET["q"]);
    echo ncn_admin_table_pagniation($url, $total_count, $num_per_page, $page, 15);
  }
?>
</div></div>
<?php
  $content = ob_get_contents();
  ob_end_clean();
  
  return $content;
}


function delete_payment_transaction($pid) {
 // $query = "DELETE FROM payment_log WHERE pid=$pid";
  $result = db_query('DELETE FROM {payment_log} WHERE pid=:a',array(':a'=>$pid));
  if (!$result) {
    drupal_set_message(t('Failed to delete payment transaction(#!pid) deleted, successfully.', array('!pid'=>$pid)), 'error');
  } else {
    drupal_set_message(t('Payment Transaction(#!pid) deleted, successfully.', array('!pid'=>$pid)));
    ncn_report_delete_payment_ncn($pid);
  }
}
  
function get_payment_from($tr) {
  $pname ='';
  if ($tr['uid'] != 0) {
    $_user = user_load($tr['uid']);
    if ($_user) {
      $pname = $_user->profile_firstname.' '.$_user->profile_lastname;
    }
  } else {
    $member = get_member_from_id($tr['member_id']);
    if (!empty($member)) {
      $pname = $member['first_name'].' '.$member['last_name'];
    } else {
      $pname .= "removed member:".$tr['member_id']."";
    }
  }
  
  if ($tr['uid'] != 0) {
    $_user = user_load($tr['uid']);
    if ($_user && is_distributor($_user) && $tr['target_mid']!=0) {
      $pname = $_user->profile_legalname;
      $member = get_member_from_id($tr['target_mid']);
      if (!empty($member)) {
        $pname .= ( ' (for '.$member['first_name'].' '.$member['last_name'].')' );
      } else {
        $pname .= "(for removed member:".$tr['target_mid'].")";
      }
    }
  }
  
  return $pname;
}  


function draw_select_transaction_type($type, $name, $select_val, $attributes) {
  $content = "<select id='$name' name=\"$name\" $attributes>";
  if ($type == 'filter') {
    $data = get_transaction_type("all");
    $selected = '';
    if ( $select_val == 'all' ) { $selected = 'selected'; }
    $content .= '<option value="all" '.$selected.' >Show All</option>';
  } else {
    $data = get_transaction_type();
  }
  for ($i=0; $i<count($data); $i++) {
    $selected = '';
    if ( ((string)$data[$i]['product_id']) == $select_val ) { $selected = 'selected'; }
    $content .= '<option value="'.$data[$i]['product_id'].'" '.$selected.' >'.$data[$i]['name'].'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;('.$data[$i]['price'].')</option>';
  }
  $content .= "</select>";
  
  return $content;
}

function draw_select_filter_expedite($name, $select_val, $attributes) {
  $content = "<select id='$name' name=\"$name\" $attributes>";
  $data = array(
    'all' => 'All', 
    'non-expedite' => 'No Expedite', 
    'expedite' => 'Expedite',     
  );
  
  foreach ($data as $key=>$val) {
    $selected = '';
    if ( $key == $select_val ) { $selected = 'selected'; }
    $content .= '<option value="'.$key.'" '.$selected.' >'.$val.'</option>';
  }
  $content .= "</select>";
  
  return $content;
}

/**
*  Download Handler (download/transaction)
*/
function ncn_admin_payment_download_transaction() {
  $transactions = ncn_admin_payment_get_downloadable_transactions('all');
  $report_name = "transaction";
  
  require_once 'sites/all/libraries/php_xls/Classes/PHPExcel.php';
  
  $demo_file = "transaction.xls";
  $objReader = PHPExcel_IOFactory::createReader("Excel5");
  $objPHPExcel = $objReader->load($demo_file);
  $count = count($transactions);
  
  if ($count>0) {
    $objPHPExcel->getActiveSheet()->insertNewRowBefore(9, $count);  
  }
  for ($i=0; $i<$count; $i++) {
    $source = "A7:G7";
    if ($i%2 == 1) {
      $source = "A8:G8";
    }
    $row_index = $i+9;
    $dest = "A$row_index:G$row_index";
    $objPHPExcel->getActiveSheet()->duplicateStyle( $objPHPExcel->getActiveSheet()->getStyle($source), $dest );  
  }
  $objPHPExcel->getActiveSheet()->removeRow(7+$count, 2);
  
  $i = 0;
  foreach ($transactions as $tr) {
    $row_index = $i+7;
    
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("A$row_index" , $tr['pid']);
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("B$row_index" , $tr['time']);
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("C$row_index" , $tr['type']);
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("D$row_index" , $tr['name']);
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("E$row_index" , $tr['company']);
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("F$row_index" , $tr['member_since']);
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue("G$row_index" , $tr['amount']);
    
    $i += 1;
  }
  
  header('Content-Type: application/vnd.ms-excel');
  header('Content-Disposition: attachment;filename="'.$report_name.'"');
  header('Cache-Control: max-age=0');
  $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
  $objWriter->save('php://output');
  exit;
}

/**
* Get Transactions in Downloadable Report
* 
* @param mixed $mode
*/
function ncn_admin_payment_get_downloadable_transactions($mode) {
  $data = array();
  
  $query = "SELECT * FROM {payment_log} WHERE status=1";
  $where = '';
  
  $query .= $where;
  $query .= " ORDER BY timestamp DESC";
  
  $result = db_query($query);
  $data_type = get_transaction_type('all');
  foreach($result as $row) 
  {
    $row = (array)$row;
    $ptype = get_item_from_key($data_type, 'ptype', $row['ptype']);
    $pname = get_payment_from($row);
    $_user = user_load($row['uid']);
    $pcompany_name = '';
    if ($row['uid'] && $_user) {
      $pcompany_name = $_user->profile_legalname;
    } else if ($row['member_id']){
      $member = get_member_from_id($row['member_id']);
      if (!empty($member)) {
        $pcompany_name = $member['legalname'];
      }
    }
    
    $str_cost = render_payment_cost($row['cost']);
    
    $member_since = '';
    
    if ( is_member($_user->profile_memberid) ) {
      $member = get_member_from_id($_user->profile_memberid);
      if (!empty($member)) {
        $member_since = $member['create'];
      }
    }
    if ($member_since == '') {
      if ($_user && $_user->created) {
        $member_since = date('m/d/Y', $_user->created);
      }
    } else {
      if ($member_since != 0) {
        $member_since = date('m/d/Y', $member_since);
      } else {
        $member_since = '';
      }
    }
  
    $transaction = array(
      'pid' => $row['pid'], 
      'time'=> date('d M, Y H:i', $row['timestamp']), 
      'type'=> $ptype['name'], 
      'name'=> $pname,
      'company' => $pcompany_name, 
      'member_since'=>$member_since, 
      'amount'=> strip_tags($str_cost), 
    );
    $data[] = $transaction;
  }
  
  return $data;
}

?>
