<?php
function ncn_admin_invoiceform_3_submit($form, &$form_state)
{
    $live = $form_state['values']['live'];
    $claim_id = arg(4);

    // we know the file was good
    $invoice_dir = 'public://invoices/'.$claim_id;
    if (is_dir($invoice_dir)===false) {
        @drupal_mkdir($invoice_dir);
    }

    $file = file_load($form_state['values']['fid']);
    $newFilename = @strtr($file->filename,array("( "=>"", " )"=>"", "("=>"", ")"=>"", " "=>"_"));
    
    if(isset($form_state['clicked_button']['#post']['current_scroll_position']))
    {
        $_SESSION['current_scroll_position'] = $form_state['clicked_button']['#post']['current_scroll_position'];
    }
    $file->status = FILE_STATUS_PERMANENT;
    $res = file_move($file, "public://invoices/{$claim_id}/{$newFilename}");
    // add invoice to database
    if ($res)
    {
        if ($live == 3 || $live == 4) {
            $res_file = convert_pdf_to_jpg($file, $claim_dir);
            if ($res_file === FALSE) {
                drupal_set_message('Failed to convert uploaded PDF file to Image', 'error');
                return;
            }
            $file = $res_file;
        }else{
            $file = $res;
        }
        
        // get last revision
        //echo $query = "SELECT * FROM claims_invoices WHERE claim_id=".$claim_id." AND `live`=".$live." ORDER BY revision DESC";
        $result = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND live=:b ORDER BY revision DESC',
        array(':a'=>$claim_id,':b'=>$live));
        $row_count = $result->rowCount();

        if($live == 11){
            $row_count = 0;
        }

        if ($row_count == 0)
        {
            $file->filepath = str_replace("public://","sites/default/files/",$file->uri);

            $revision = 0;  
        //  $query = "INSERT INTO claims_invoices VALUES(NULL, ".$claim_id.", ".$revision.", \"".AddSlashes($file->filename)."\", \"".AddSlashes($file->filepath)."\", \"".AddSlashes($file->filemime)."\", ".date('U').", ".$live.");";
            $query = db_query('INSERT INTO {claims_invoices} VALUES(NULL,:a,:b,:c,:d,:e,:f,:g)',
                array(':a'=>$claim_id,':b'=>$revision,':c'=>$file->filename,':d'=>$file->filepath,':e'=>$file->filemime,
                    ':f'=>date('U'),':g'=>$live));
        }
        else
        {
            //$row = (array)$result;
            //$revision = $row['revision']+1;
            $row = $result->fetchAssoc();
            $revision = 0;

            /*Create File Object from File path*/
            $uri = str_replace("sites/default/files/","public://",$row['filepath']);
            $fileObj = createFileObject($uri);

            file_delete($fileObj);
            $file->filepath = str_replace("public://","sites/default/files/",$file->uri);

            $query = db_query('UPDATE {claims_invoices} SET filename=:a, filepath=:b, filemime=:c,
                timestamp=:d WHERE claim_id=:e AND live=:f',array(':a'=>$file->filename,':b'=>$file->filepath,
                ':c'=>$file->filemime,':d'=>date('U'),':e'=>$claim_id,':f'=>$live));
        }
        
        $result = $query;
        
        // success!
        $claim_file_title = ncn_admin_get_claim_file_title($live);
        drupal_set_message( $claim_file_title.' "'.$file->filename.'" was uploaded.', 'status');    
        $log_message = t("!user_name uploaded !file_title  : !file_name", array('!user_name'=>ncn_amin_get_user_role_and_name(), '!file_title'=>$claim_file_title, '!file_name'=>$file->filename));
        ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);
    }
}
function ncn_admin_invoiceform_4_submit($form, &$form_state)
{
    ncn_admin_invoiceform_3_submit($form, $form_state);
}
function ncn_admin_invoiceform_5_submit($form, &$form_state)
{
    ncn_admin_invoiceform_3_submit($form, $form_state);
}


//------------------------------------------------------------------------------
function ncn_admin_invoiceform_2_submit($form, &$form_state)
{
    ncn_admin_invoiceform_1_submit($form, $form_state);
}
function ncn_admin_invoiceform_10_submit($form, &$form_state)
{
    ncn_admin_invoiceform_3_submit($form, $form_state);
}
function ncn_admin_invoiceform_11_submit($form, &$form_state)
{
    ncn_admin_invoiceform_3_submit($form, $form_state,'esx');
}
function ncn_admin_invoiceform_12_submit($form, &$form_state)
{
    ncn_admin_invoiceform_3_submit($form, $form_state);
}

// upload invoice form SUBMIT
function ncn_admin_invoiceform_1_submit($form, &$form_state)
{
    $live = $form_state['values']['live'];
    
    if(isset($form_state['clicked_button']['#post']['current_scroll_position']))
    {
        $_SESSION['current_scroll_position'] = $form_state['clicked_button']['#post']['current_scroll_position'];
    } 
    
    $claim_id = arg(4);
    
    // we know the file was good
    //$file = file_save_upload('invoice_upload'.$live);
    $file = file_load($form_state['values']['fid']);
    $newFilename = @strtr($file->filename,array("( "=>"", " )"=>"", "("=>"", ")"=>"", " "=>"_"));
    
    $filePath = "sites/default/files/invoices/$claim_id/";
    $claim_dir = DRUPAL_ROOT."/".$filePath;
    
    if (!is_dir($claim_dir)) {
        mkdir($claim_dir, 0755);
    }
    
    $file->status = FILE_STATUS_PERMANENT;
    $res = file_move($file, "public://invoices/{$claim_id}/{$newFilename}", FILE_EXISTS_RENAME);
    
    // add invoice to database
    if ($res)
    {
        // get last revision
    //  $query = "SELECT revision FROM claims_invoices WHERE claim_id=".$claim_id." AND `live`=".$live." ORDER BY revision DESC LIMIT 0,1";
        $result = db_query('SELECT revision FROM {claims_invoices} WHERE claim_id=:a AND live=:b ORDER BY revision DESC LIMIT 0,1',
        array(':a'=>$claim_id,':b'=>$live));

        $row_count = $result->rowCount();
        
        if ($row_count == 0)
        {   $revision = 0;  }
        else
        {
            $row = $result->fetchAssoc();
            $revision = $row['revision']+1;
        }

        // insert new data
    //  $query = "INSERT INTO claims_invoices VALUES(NULL, ".$claim_id.", ".$revision.", \"".AddSlashes($file->filename)."\", \"".AddSlashes($file->filepath)."\", \"".AddSlashes($file->filemime)."\", ".date('U').", ".$live.");";
        $result = db_query('INSERT INTO {claims_invoices} VALUES(NULL,:a,:b,:c,:d,:e,:f,:g)',
            array(':a'=>$claim_id,':b'=>$revision,':c'=>$res->filename,':d'=>str_replace("public://","sites/default/files/",$res->uri),
                ':e'=>$res->filemime,':f'=>date('U'),':g'=>$live));


        $claim_file_title = ncn_admin_get_claim_file_title($live);
        // add entry to logfile
        
        $log_message = ncn_amin_get_user_role_and_name()." uploaded new file: ".$file->filename." ( $claim_file_title - revision #".$revision. " )";
    ncn_admin_insert_claim_log($claim_id, date('U'), $log_message);     
        
        // success!
        $log_message = "Uploaded new file: ".$file->filename." ( $claim_file_title - revision #".$revision. " )";
        drupal_set_message($log_message, 'status');
        
        if ($live == 1 && ncn_admin_is_claim_first_free($claim_id) ) {
            $base_abs_path = $_SERVER['DOCUMENT_ROOT'];
            $final_invoice_name = $file->filepath;
            //$command = "java -jar C:/pdfwatermark/ncn_pdf_watermark.jar $base_abs_path/$final_invoice_name";
            $command = "java -jar ../pdfwatermark/ncn_pdf_watermark.jar $base_abs_path/$final_invoice_name";

            $output = shell_exec($command);
            //$final_locked_invoice_name = ncn_admin_get_locked_invoice_filename($final_invoice_name);
        }
    }
}

//------------------------------------------------------------------------------
function ncn_admin_invoiceform_3_validate($form, &$form_state)
{
    $live = $form_state['values']['live'];

    // check file was uploaded
    $file = file_save_upload('invoice_upload'.$live);

    if (!$file)
    {
        // If you want to require it, you'll want to do it here... something like this:
        form_set_error('invoice_upload'.$live, 'File missing for upload.');
        $form_state['values']['fid'] = 0;
    } else {
        $form_state['values']['fid'] = $file->fid;
    }

    switch ($live)
    {
        case 3:
        case 4:
        case 5: if ($file->filemime != "application/pdf") {
                    form_set_error('invoice_upload'.$live, 'Only PDF file could be uploaded.');
                }
        break;
    }
}

function ncn_admin_invoiceform_4_validate($form, &$form_state)
{
    ncn_admin_invoiceform_3_validate($form, $form_state);
}
function ncn_admin_invoiceform_5_validate($form, &$form_state)
{
    ncn_admin_invoiceform_3_validate($form, $form_state);
}
function ncn_admin_invoiceform_10_validate($form, &$form_state)
{
    ncn_admin_invoiceform_3_validate($form, $form_state);
}
function ncn_admin_invoiceform_11_validate($form, &$form_state)
{
    ncn_admin_invoiceform_3_validate($form, $form_state);
}
function ncn_admin_invoiceform_12_validate($form, &$form_state)
{
    ncn_admin_invoiceform_3_validate($form, $form_state);
}

function ncn_admin_invoiceform_2_validate($form, &$form_state)
{
    ncn_admin_invoiceform_1_validate($form, $form_state);
}

// upload invoice form VALIDATE
function ncn_admin_invoiceform_1_validate($form, &$form_state)
{
    $live = $form_state['values']['live'];

    /*if(isset($form_state['#post']['current_scroll_position']))
    {
        $_SESSION['current_scroll_position'] = $form_state['#post']['current_scroll_position'];
    } */

    // check file was uploaded
    $file = file_save_upload('invoice_upload'.$live);

    if (!$file)
    {
        // If you want to require it, you'll want to do it here... something like this:
        form_set_error('invoice_upload'.$live, 'File missing for upload.');
        $form_state['values']['fid'] = 0;       
    } else {
        $form_state['values']['fid'] = $file->fid;
    }  
}


//------------------------------------------------------------------------------
// upload invoice form display
function ncn_admin_invoiceform_1($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload1'] = array(
        '#type' => 'file',
        '#title' => t(''),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 1,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload Final Invoice'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}

//------------------------------------------------------------------------------
// upload invoice form display
function ncn_admin_invoiceform_2($form_state)
{

    $form = array(); //'#onclick' => t('return on_set_scroll_position();'), 
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload2'] = array(
        '#type' => 'file',
        '#title' => t(''),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 2,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload Claim Documents'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));
    
    return($form);

}
//------------------------------------------------------------------------------
// upload invoice form display
function ncn_admin_invoiceform_3($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload3'] = array(
        '#type' => 'file',
        '#title' => t(''),
        '#attributes' => array('accept'=> 'application/pdf'),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 3,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload Service Contract'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}
//------------------------------------------------------------------------------
// upload invoice form display
function ncn_admin_invoiceform_4($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload4'] = array(
        '#type' => 'file',
        '#title' => t(''),
        '#attributes' => array('accept'=> 'application/pdf'),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 4,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload Certificate of Completion'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}
//------------------------------------------------------------------------------
// upload invoice form display
function ncn_admin_invoiceform_5($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload5'] = array(
        '#type' => 'file',
        '#title' => t(''),
        '#attributes' => array('accept'=> 'application/pdf'),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 5,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload Completed Invoice'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}
//------------------------------------------------------------------------------
// upload ce claim file
function ncn_admin_invoiceform_10($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload10'] = array(
        '#type' => 'file',
        '#title' => t(''),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 10,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload CE Documents'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}
//------------------------------------------------------------------------------
// upload ce esx file
function ncn_admin_invoiceform_11($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload11'] = array(
        '#type' => 'file',
        '#title' => t(''),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 11,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );


    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload ESX File'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}
//------------------------------------------------------------------------------
// upload invoice form display
function ncn_admin_invoiceform_12($form_state)
{

    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice_upload12'] = array(
        '#type' => 'file',
        '#title' => t(''),
    );
    
    $form['live'] = array(
        '#type' => 'hidden',
        '#value' => 12,
    );

    $form['current_scroll_position'] = array(
        '#type' => 'hidden',
        '#attributes' => array('class' => t('current_scroll_position')),
        '#value' => t(''),
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Upload CE Invoice PDF File'), '#attributes' => array('onclick' => 'return on_set_scroll_position()'));

    return($form);

}

function ncn_admin_invoice_data($claim_id, $data, $pvc_disabled='') {
    global $base_url;
    if (!isset($data['data_entered']))              { $data['data_entered'] = ''; }
    if (!isset($data['price_list']))                { $data['price_list'] = ''; }
    if (!isset($data['iop_number']))                { $data['iop_number'] = ''; }
    if (!isset($data['insurance_company']))         { $data['insurance_company'] = ''; }
    if (!isset($data['claim_number']))              { $data['claim_number'] = ''; }
    if (!isset($data['claim_adjuster']))            { $data['claim_adjuster'] = ''; }
    if (!isset($data['claim_adjuster_phone']))      { $data['claim_adjuster_phone'] = ''; }
    if (!isset($data['claim_adjuster_fax']))        { $data['claim_adjuster_fax'] = ''; }
    if (!isset($data['start_date']))                { $data['start_date'] = ''; }
    if (!isset($data['completion_date']))           { $data['completion_date'] = ''; }
?>
<script type="text/javascript">
jQuery(function() {
    ncn_admin_invoice_data_datepicker('#invoice_extended_data #start_date');
    ncn_admin_invoice_data_datepicker('#invoice_extended_data #completion_date');
});
function ncn_admin_invoice_data_datepicker(objStr) {
    jQuery( objStr ).datepicker({
        showOn: "button",
        buttonImage: "<?php echo $base_url."/".drupal_get_path('module', 'ncn_report')."/images/calendar.gif"; ?>",
        buttonImageOnly: true,
        dateFormat: 'mm/dd/yy',
    });
}
</script>
<fieldset class="">
    <legend>Claim Invoice Data</legend>
    <div>
    <form id="invoice_extended_data" method="post" style=" margin:0px; ">
        <input type="hidden" name="extend_data_function" value="extend_data" />
        <input type="hidden" name="current_scroll_position" class="current_scroll_position" value="" />
        <table style="border: solid 0px;">
        <tbody style="border:0px;">
            <tr class="even"><td><label for="claim_invoice_type">Claim Invoice Type: &nbsp;</label></td><td><?php echo draw_select_claim_invoice_type('claim_invoice_type', $data['claim_invoice_type']); ?></td></tr>
            
            <tr class="odd"><td><label for="company_fax_number">Company Fax Number: &nbsp;</label></td><td><input id="company_fax_number" name="company_fax_number" size="20" value="<?php echo $data['company_fax_number']; ?>" /></td></tr>

            <tr class="even"><td width="30%"><label for="data_entered">Data Entered: &nbsp;</label></td><td><input id="data_entered" name="data_entered" size="20" value="<?php echo $data['data_entered']; ?>" /></td></tr>
            <tr class="odd"><td width="30%"><label for="price_list">Price List: &nbsp;</label></td><td><input id="price_list" name="price_list" size="20" value="<?php echo $data['price_list']; ?>"  /></td></tr>
            <tr class="even"><td width="40%"><label for="iop_type">Insured's Other Phone: &nbsp;</label></td><td>
                <select id="iop_type" name="iop_type" >
                    <option value="Work" <?php if($data['iop_type']=="Work") {echo "selected";}?> >-Work-</option>
                    <option value="Mobile" <?php if($data['iop_type']=="Mobile") {echo "selected";}?> >-Mobile-</option>
                </select>
                <input id="iop_number" name="iop_number" size="12" value="<?php echo $data['iop_number']; ?>"  />
            </td></tr>
            <tr class="odd"><td><label for="type_of_loss">Type of Loss: &nbsp;</label></td><td>
                <select id="type_of_loss" name="type_of_loss" style="width: 100px;"  >
                    <option value="Water" <?php if($data['type_of_loss']=="Water") {echo "selected";}?>>- Water -</option>
                    <option value="Mold" <?php if($data['type_of_loss']=="Mold") {echo "selected";}?>>- Mold -</option>
                </select>
            </td></tr>
            <tr class="even"><td><label for="insurance_company">Insurance Company: &nbsp;</label></td><td><input id="insurance_company" name="insurance_company" size="20" value="<?php echo $data['insurance_company']; ?>"  /></td></tr>
            <tr class="odd"><td><label for="claim_number">Claim Number: &nbsp;</label></td><td><input id="claim_number" name="claim_number" size="20" value="<?php echo $data['claim_number']; ?>"  /></td></tr>
            
            <tr class="even" ><td><label for="claim_adjuster">Claim Adjuster: &nbsp;</label></td><td><input id="claim_adjuster" name="claim_adjuster" size="20" value="<?php echo $data['claim_adjuster']; ?>"  /></td></tr>
            <tr class="odd"><td><label for="claim_adjuster_phone">Claim Adjuster Phone: &nbsp;</label></td><td>
                <select id="cap_type" name="cap_type" >
                    <option value="Work" <?php if($data['cap_type']=="Work") {echo "selected";}?> >-Work-</option>
                    <option value="Mobile" <?php if($data['cap_type']=="Mobile") {echo "selected";}?> >-Mobile-</option>
                </select>
                <input id="claim_adjuster_phone" name="claim_adjuster_phone" size="20" value="<?php echo $data['claim_adjuster_phone']; ?>"  /></td></tr>
            <tr class="even"><td><label for="claim_adjuster_fax">Claim Adjuster Fax: &nbsp;</label></td><td><input id="claim_adjuster_fax" name="claim_adjuster_fax" size="20" value="<?php echo $data['claim_adjuster_fax']; ?>" /></td></tr>

            <tr class="odd"><td><label for="category">Category: &nbsp;</label></td><td>
                <select id="category" name="category" style="width: 100px;"  >
                    <option value="1" <?php if($data['category']=="1") {echo "selected";}?> >-1-</option>
                    <option value="2" <?php if($data['category']=="2") {echo "selected";}?> >-2-</option>
                    <option value="3" <?php if($data['category']=="3") {echo "selected";}?> >-3-</option>
                </select>
            </td></tr>
            <tr class="even"><td><label for="start_date">Start Date: &nbsp;</label></td><td><input id="start_date" name="start_date" size="20" value="<?php echo $data['start_date']; ?>" /></td></tr>
            <tr class="odd"><td><label for="completion_date">Completion Date: &nbsp;</label></td><td><input id="completion_date" name="completion_date" size="20" value="<?php echo $data['completion_date']; ?>" /></td></tr>
            <tr class="even"><td><label for="cause" style="vertical-align: top;">Cause:&nbsp;</label></td><td><textarea id="cause" name="cause" style="width: 90%; height: 100px;" ><?php echo isset($data['cause'])? $data['cause']:''; ?></textarea></td></tr>
            <?php if ($pvc_disabled!='disabled'): ?>
                <tr class="odd"><td colspan="2"><input type="submit" onclick="return on_set_scroll_position();" value="  Save  "/></td></tr>
            <?php endif; ?>
        </tbody>
        </table>
    </form>
    </div>
</fieldset>
<?php
}

function ncn_admin_draw_invoice_upload($claim_id, $type, $pvc_disabled='')
{
    GLOBAL $base_url;
    ob_start();
    $legend_title = "";
    switch ($type)
    {
        case "1":
            $legend_title = "Upload Final Invoice";
        break;

        case "2":
            $legend_title = "Upload Claim Documents";
        break;
        
        case "3":
            $legend_title = "Upload Service Contract";
        break;
        case "4":
            $legend_title = "Upload Certificate of Completion";
        break;
        case "5":
            $legend_title = "Upload Completed Invoice";
        break;
        
        case "10":
            $legend_title = "Upload CE Documents";
            break;
        case "11":
            $legend_title = "Upload ESX File";
            break;
        case "12":
            $legend_title = "Upload CE Invoice PDF File";
            break;

    }

?>
    <div>
        <fieldset class="">
            <legend class="">
                <?php echo $legend_title; ?>
            </legend>
            <div class="">
            <?php
                if ($pvc_disabled!='disabled') {
                    $ncn_admin_invoiceform = drupal_get_form('ncn_admin_invoiceform_'.$type);
                    print drupal_render($ncn_admin_invoiceform);
                }
            ?>
            
                <div id="invoice_revisions">
                <table>
                <tbody>
                    <thead class="tableHeader-processed">
                    <tr>
                        <th>Revision</th>
                        <th>File</th>
                        <th>Timestamp</th>
                        <th>Action</th>
                    </tr>
                    </thead>
                    
                    <?php
                    // get latest 5 invoice revisions
                    
                //  $query = "SELECT * FROM claims_invoices WHERE claim_id=".$claim_id." AND `live`=".$type." ORDER BY revision DESC LIMIT 0,5";
                    $result = db_query('SELECT * FROM {claims_invoices} WHERE claim_id=:a AND `live`= :b ORDER BY revision DESC,invoice_id DESC LIMIT 0,5',array(':a'=>$claim_id ,':b'=>$type));
                    $row_count = $result->rowCount();
                    
                    // run through invoice revisions
                    //for ($i=0; $i<$row_count; $i++)
                    $result_rec = $result->fetchAll();
                    $i = 0;
                    foreach($result as $row)
                    {
                        $row = (array)$row;
                        $id = uniqid();
                        
                        if ($row['revision'] == 0)  // don't want to show "revision #0")
                        {   $revision = ""; }
                        else
                        {   $revision = " Rev. #".$row['revision']; }

                        ?>              
                        <tr class="<?php if ($i%2) print "even"; else print "odd"; ?>">
                            <td><?= $revision; ?></td>
                            <td><img src="<?= $base_url; ?>/sites/all/modules/ncn_claims_manager/images/pdf_icon_16.gif" width="16" height="16" align="left" />&nbsp;<a href="<?= $base_url; ?>/account/serve_invoice_file/<?= $claim_id; ?>/<?= $row['live']; ?>/<?= $row['revision']; ?>/<?= $row['invoice_id']; ?>"><?= StripSlashes($row['filename']); ?></a></td>
                            <td><?= date("m/d/Y H:i:s", $row['timestamp']); ?></td>
                            <td><?php if ($pvc_disabled!='disabled'): ?>
                                <a class='invoice_file_<?=$id?>' href="<?= $base_url; ?>/account/delete_invoice_file/<?= $claim_id; ?>/<?= $row['live']; ?>/<?= $row['revision']; ?>/<?= $row['invoice_id']; ?>" onclick='set_scroll_position("invoice_file_<?=$id ?>"); return confirm("Are you sure you want to delete this file?");'>[delete]</a>
                                <?php endif; ?>
                            </td>
                        </tr>
                        <?php
                        $i++;
                    }
                    
                    // no results
                    if ($row_count == 0)
                    {
                        ?>
                        <tr>
                            <td coslpan=3>No files uploaded</td>
                        </tr>
                        <?php
                    }
                                
                    ?>
                </tbody>
                </table>
                </div>
            </div>
        </fieldset>
            
        

    </div>
    <?php

    $content = ob_get_contents();
    ob_end_clean();
    
    return $content;
}

//------------------------------------------------------------------------------
// upload invoice form
function ncn_admin_upload_invoice($form_state)
{
    $form = array();
    $form['#attributes'] = array('enctype' => "multipart/form-data");

    $form['invoice'] = array(
        '#type' => 'file',
        '#title' => t("New Claim Invoice"),
        '#tree' => FALSE,
        '#size' => 40,
    );

    $form['next'] = array('#type' => 'submit', '#weight' => 101, '#value' => t('Save Invoice'));
    return $form;
    
}
