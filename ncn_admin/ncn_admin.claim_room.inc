<?php
function ncn_admin_edit_roomname()
{
    $claim_id = $_POST['claim_id'];
    $old_roomname = $_POST['old_roomname'];
    $new_roomname = $_POST['new_roomname'];
    $j_result = ncn_admin_photo_rename_room($claim_id, $old_roomname, $new_roomname);
    echo json_encode($j_result);
}

function ncn_admin_photo_rename_room($claim_id, $old_roomname, $new_roomname)
{
    $new_roomname = trim(preg_replace("/[^a-zA-Z0-9\s]/", "", $new_roomname));
    $error = false;
    $j_result['msg'] = '';
    $j_result['code'] = "success";
//  $query = "SELECT * FROM claims_data WHERE claim_id=" . $claim_id . " AND `key`='scope'";
    $result = db_query('SELECT * FROM {claims_data} WHERE claim_id=:a AND `key`=:b',array(':a'=>$claim_id,':b'=>'scope'));
    if ($result->rowCount() == 0)
    {
        $error = true;
        $j_result['msg'] = t('Claim(#!claim_id) doesn\'t exist.', array(
            '!claim_id' => $claim_id
        ));
        $j_result['code'] = 'NCN_ADMIN_001_EDIT_ROOMNAME';
    }
    else
    if ($old_roomname != $new_roomname)
    {
        $row = (array)$result->fetchAssoc();
        $row['val'] = unserialize($row['val']);
        if (isset($row['val'][$new_roomname]))
        {
            $error = true;
            $j_result['msg'] = t('Roomname(!roomname) is duplicated.', array(
                '!roomname' => $new_roomname
            ));
            $j_result['code'] = 'NCN_ADMIN_005_EDIT_ROOMNAME';
        }
        else
        {
            $new_scope = array();
            $find = false;
            foreach($row['val'] as $roomname => $roomdata)
            {
                if ($roomname == $old_roomname)
                {
                    $new_scope[$new_roomname] = $roomdata;
                    $find = true;
                }
                else
                {
                    $new_scope[$roomname] = $roomdata;
                }
            }

            if (!$find)
            {
                $error = true;
                $j_result['msg'] = t('Claim(#!claim_id) hasn\'t a room(!roomname).', array(
                    '!claim_id' => $claim_id,
                    '!roomname' => $old_roomname
                ));
                $j_result['code'] = 'NCN_ADMIN_002_EDIT_ROOMNAME';
            }
            else
            {
                $new_scope = serialize($new_scope);
                $scope = $new_scope;
            //  $query = "UPDATE claims_data SET `val`='" . mysql_real_escape_string($new_scope) . "' WHERE claim_id=$claim_id AND `key`='scope'";
                $result = db_query('UPDATE {claims_data} SET val=:a WHERE claim_id=:b AND `key`=:c',array(':a'=>$scope,':b'=>$claim_id,':c'=>'scope'));
                if ($result->rowCount()==0)
                {
                    $error = true;
                    $j_result['msg'] = t('Failed to rename room(!roomname).', array(
                        '!roomname' => $old_roomname
                    ));
                    $j_result['code'] = 'NCN_ADMIN_003_EDIT_ROOMNAME';
                }
                else
                {
                    ncn_admin_update_room_data_by_renaming_room($claim_id, $old_roomname, $new_roomname);
                    $log_message = t('Room(!roomname) renamed to !new_roomname, successfully.', array(
                        '!roomname' => $old_roomname,
                        '!new_roomname' => $new_roomname
                    ));
                    $j_result['msg'] = $log_message;
                    $log_message = t('!user_name renamed room(!roomname) to !new_roomname.', array(
                        '!user_name' => ncn_amin_get_user_role_and_name() ,
                        '!roomname' => $old_roomname,
                        '!new_roomname' => $new_roomname
                    ));
                    ncn_admin_insert_claim_log($claim_id, date('U') , $log_message);
                }
            }
        }
    }

    return $j_result;
}

function ncn_admin_update_room_data_by_renaming_room($claim_id, $room_name, $new_name)
{

    // Image Captions
    $result = db_query('UPDATE {claim_img_captions} SET roomname=:a WHERE claim_id=:b AND roomname=:c',
        array(':a'=>$new_name,':b'=>$claim_id,':c'=>$room_name));

    // Scope Image
    $result = db_query('UPDATE {claims_scope} SET roomname=:a WHERE claim_id=:b AND roomname=:c',
        array(':a'=>$new_name,':b'=>$claim_id,':c'=>$room_name));

    // Scopesheet Data
    $result = db_query('UPDATE {claims_room_data_extra} SET room_name=:a WHERE claim_id=:b AND room_name=:c',
        array(':a'=>$new_name,':b'=>$claim_id,':c'=>$room_name));
}

function _get_image_captions($claim_id, $roomname, $img_count){
//  $query = "SELECT * FROM claim_img_captions WHERE claim_id=$claim_id AND roomname = '$roomname'";
    $result = db_query('SELECT * FROM {claim_img_captions} WHERE claim_id=:a AND roomname = :b',
        array(':a'=>$claim_id,':b'=>$roomname));
    if ($result->rowCount()>0) {
        $row = $result->fetchAssoc();
        $data = unserialize($row['data']);
        return $data;
    }
    
    // set default values
    $data = array();
    if ($img_count == 3) {
        $data[0] = 'Exterior of Dwelling';
        $data[1] = 'Cause & Origin';
        $data[2] = 'Driver\'s License';
    } else {
        $data[0] = 'Pre Mitigation';
        $data[1] = 'Mitigation';
        $data[2] = 'Post Mitigation';
        
        $data[3] = 'Before Reading';
        $data[4] = 'During Reading';
        $data[5] = 'After Reading';
    }
    
    return $data;
    
    
}

function _save_image_captions($claim_id, $roomname, $data){
//  $query = "SELECT * FROM claim_img_captions WHERE claim_id=$claim_id AND roomname = '$roomname'";
    $result = db_query('SELECT * FROM {claim_img_captions} WHERE claim_id=:a AND roomname = :b',
        array(':a'=>$claim_id,':b'=>$roomname));
    $data = serialize($data);
    if ($result->rowCount()>0) {
        $query1 = db_query('UPDATE {claim_img_captions} SET data=:a WHERE claim_id=:b AND roomname=:c',
            array(':a'=>$data,':b'=>$claim_id ,':c'=>$roomname));
    } else {
        $query1 = db_query('INSERT INTO {claim_img_captions} (claim_id, roomname, data) VALUES (:a,:b,:c)',
            array(':a'=>$claim_id,':b'=>$roomname,':c'=>$data));
    }
    $result1 = $query1;
    if (!$result1) {
        return FALSE;
    }
    return TRUE;
    
}
/**
 * type : 3 or 6
 */
function _get_blank_room_image_url($type, $index){
    global $base_url;
    $image_url = $base_url.'/'.drupal_get_path('module', 'ncn_create_claim').'/images/';
    if ($type == 3) {
        switch ($index) {
            case 0: $image_url .= 'fronhouse.png';  break;
            case 1: $image_url .= 'origin.png';     break;
            case 2: $image_url .= 'license.png';    break;
        }
    } else {
        $image_url .= ('blank'.($index+1).'.png');
    }
    
    return $image_url;
}
    
function convert_pdf_to_jpg($file, $dir_url) {
    
    $old_file_path = $file->filepath;
    
    $base_abs_path = $_SERVER['DOCUMENT_ROOT'];
    $pdf_full_path = $base_abs_path."/".$file->filepath;
    $folder_full_path = $base_abs_path."/".$dir_url;
    $jpg_prefix = $file->filename."-";
    
    
    //$command = "java -jar c:/AspriseJavaPDF-free.jar $pdf_full_path $folder_full_path $jpg_prefix";
    $command = "java -jar ../AspriseJavaPDF-free.jar $pdf_full_path $folder_full_path $jpg_prefix";
    
    //$command = "c:/pdf2image -o $folder_full_path $pdf_full_path --format jpg --prefix $jpg_prefix --verb 3 ";
    //$command = "./../pdf2image -o $folder_full_path $pdf_full_path --format jpg --prefix $jpg_prefix --verb 3 ";
                                         
    $output = shell_exec($command);
    //echo $output;
    //exit;
    file_delete($file);
    $error = false;
    $file->filepath .=".jpg";
    $file->filename .=".jpg";
    $file->filemime = "image/jpg";
    if (!is_file("$dir_url/$jpg_prefix"."1.jpg")){
    
    if (is_file("$dir_url/$jpg_prefix".".jpg")){
        $res = copy("$dir_url/$jpg_prefix".".jpg" , $file->filepath);
        if (!$res) { $error=true; }
        } else {
            return FALSE;       
        }
    } else {
        $res = copy("$dir_url/$jpg_prefix"."1.jpg" , $file->filepath);
        if (!$res) { $error=true; }
    }
    
    if (is_file("$dir_url/$jpg_prefix".".jpg")){
        file_delete("$dir_url/$jpg_prefix".".jpg");
    }
    
    for ($i=1; $i<300; $i++) {
        if (is_file("$dir_url/$jpg_prefix"."$i.jpg")){
            file_delete("$dir_url/$jpg_prefix"."$i.jpg");
        } else {
            break;
        }
    }   
    if ($error) {
        return FALSE;
    }   
    return $file;
}

function convert_bmp_to_jpg($file) {

    $filepath = drupal_realpath($file->uri);
    $mi = ImageCreateFromBMP($filepath);
    $file->uri .= ".jpg";
    $file->filename .= ".jpg";
    $file->filemime = "image/jpg";
    $result = imagejpeg($mi,$filepath);
    
    if ($result) {
        return $file;
    }
    return FALSE;
}
