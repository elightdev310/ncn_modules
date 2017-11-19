<?php

/**
 * Insert record to chatter_post table
 */
function ncn_chatter_insert_post_record($data=array()) {
    $result = db_insert('chatter_post')
            ->fields(array(
                'content'   => $data['content'], 
                'ncn_only'  => (isset($data['ncn_only'])? $data['ncn_only'] : 0), 
                'fid'       => $data['file_id'], 
                'uid'       => $data['uid'], 
                'created_at'=> (isset($data['created_at'])? $data['created_at'] : date('U')), 
                'updated_at'=> (isset($data['updated_at'])? $data['updated_at'] : date('U')), 
            ))
            ->execute();
    return $result;
}

/**
 * Insert record to chatter_comment table
 */
function ncn_chatter_insert_comment_record($data=array()) {
    $cid = db_insert('chatter_comment')
            ->fields(array(
                'content'   => $data['content'], 
                'uid'       => $data['uid'], 
                'fid'       => $data['file_id'], 
                'pid'       => $data['pid'], 
                'created_at'=> (isset($data['created_at'])? $data['created_at'] : date('U')), 
            ))
            ->execute();
    return $cid;
}

/**
 * Update updated_at of Post
 */
function ncn_chatter_post_update_time($pid, $timestamp) {
    $result = db_update('chatter_post')
            ->fields(array(
                'updated_at' => $timestamp, 
            ))
            ->condition('id', $pid)
            ->execute();
    return $result;
}


/**
 * Create Post
 * @param   $data:array( content, file_id, ncn_only: optional )
 */
function ncn_chatter_create_post($uid, $fid, $data=array()) {
    if (!$uid || !$fid) {
        return false;
    }
    
    $data['uid'] = $uid;
    $pid = ncn_chatter_insert_post_record($data);   // Insert record to table
    if (!$pid) {
        return false;
    }
    ncn_chatter_insert_feed_post_record(array('fid'=>$fid, 'pid'=>$pid));   // Feed-Post

    // Track Post in User Feed
    $user_feed = ncn_chatter_get_feed_by_entity(FEED_TYPE_USER, $uid);
    if ($user_feed) {
        if ($user_feed['fid'] != $fid) {
            ncn_chatter_insert_feed_post_record(array('fid'=>$user_feed['fid'], 'pid'=>$pid));   // User Feed - Post
        }
    }

    // TO DO: Check @tag in post
    return $pid;
}

/**
 * Create Comment
 * @param   $data:array( content, pid, file_id )
 */
function ncn_chatter_create_comment($uid, $data=array()) {
    $data['uid'] = $uid;
    $cid = ncn_chatter_insert_comment_record($data);   // Insert record to table

    if (!$cid) {
        return false;
    }

    $post_id = $data['pid'];
    // ncn_chatter_post_update_time($post_id, date('U'));  // change update_at of Post

    // Track Post in User Feed
    $user_feed = ncn_chatter_get_feed_by_entity(FEED_TYPE_USER, $uid);
    if ($user_feed) {
        ncn_chatter_insert_feed_post_record(array('fid'=>$user_feed['fid'], 'pid'=>$post_id));   // User Feed - Post
    }

    // TO DO: Check @tag in comment

    return $cid;
}

/**
 * Get Comments under Post
 */
function ncn_chatter_get_comments_under_post($pid, $count=false) {
    $result = db_select('chatter_comment', 'c')
        ->fields('c')
        ->condition('pid', $pid)
        ->orderBy('created_at', 'ASC')
        ->execute();

    if ($count) {
        return $result->rowCount();
    }

    $comments = array();
    while ($comment = $result->fetchAssoc()) {
        $comments[$comment['id']] = $comment;
    }
    return $comments;
}


/**
 * Like Post
 * @param  $entity_type 0: Post, 1: Comment
 *         $action 1: LIKE, 0: UNLIKE
 */
function ncn_chatter_like_entity($uid, $entity_id, $entity_type, $action) {
    if ($action == 1) {
        // Like
        if (!ncn_chatter_check_user_like_entity($uid, $entity_id, $entity_type)) {
            $result = db_insert('chatter_post_like')
            ->fields(array(
                'uid'           => $uid, 
                'entity_id'     => $entity_id, 
                'entity_type'   => $entity_type,
                'like_at'       => date('U')
            ))
            ->execute();
        }
        return true;
    } else {
        // Unlike
        $num_deleted = db_delete('chatter_post_like')
            ->condition('uid',          $uid)
            ->condition('entity_id',    $entity_id)
            ->condition('entity_type',  $entity_type)
            ->execute();
        return true;
    }
}
function ncn_chatter_check_user_like_entity($uid, $entity_id, $entity_type) {
    $count = db_query("SELECT COUNT(*) FROM {chatter_post_like}
                        WHERE uid=:uid AND entity_id=:entity_id AND entity_type=:entity_type", 
                        array('uid'=>$uid, 'entity_id'=>$entity_id, 'entity_type'=>$entity_type))
            ->fetchField();
    return $count;
}










////////////////////////////////////////////////////////////////////////////////

/**
 * Callback: chatter/add/post/%
 */
function ncn_chatter_add_post_action($fid) {
global $user;
    $result = array(
        'status' => 'success', 
        'msg' => '', 
    );

    $content = trim($_REQUEST['content']);
    $file_id = $_REQUEST['chatter_file'];
    $ncn_only= (isset($_REQUEST['post_filter']))? $_REQUEST['post_filter']:0;

    if ($content || $file_id) {
        if ($file_id) {
            $file = file_load($file_id);
            $dest = "public://chatter/".$user->uid;
            file_prepare_directory($dest, FILE_CREATE_DIRECTORY);
            $dest .= "/".$file->filename;
            if ($moved_file = file_move($file, $dest)) {
                $file_id = $moved_file->fid;
            } else {
                $file_id = 0;
            }
        }
        
        $pid = ncn_chatter_create_post($user->uid, $fid, array(
            'content' => $content, 
            'file_id' => $file_id,
            'ncn_only'=> $ncn_only,
        ));

        if (!$pid) {
            $result['status'] = 'error';
            $result['msg'] = t("Failed to add post. Please try again after reloading page");
        }
    } else {
        $result['status'] = 'warning';
    }
    echo json_encode($result);
}

/**
 * Callback: chatter/post/%/add/comment
 */
function ncn_chatter_add_comment_action($pid) {
global $user;
    $result = array(
        'status' => 'success', 
        'msg' => '', 
    );

    $content = trim($_REQUEST['content']);
    $file_id = $_REQUEST['chatter_file'];

    if ($content || $file_id) {
        if ($file_id) {
            $file = file_load($file_id);
            $dest = "public://chatter/".$user->uid;
            file_prepare_directory($dest, FILE_CREATE_DIRECTORY);
            $dest .= "/".$file->filename;
            if ($moved_file = file_move($file, $dest)) {
                $file_id = $moved_file->fid;
            } else {
                $file_id = 0;
            }
        }

        $cid = ncn_chatter_create_comment($user->uid, array(
            'pid'     => $pid, 
            'content' => $_REQUEST['content'], 
            'file_id' => $file_id,
        ));

        if (!$cid) {
            $result['status'] = 'error';
            $result['msg'] = t("Failed to add comment. Please try again after reloading page");
        } else {
            // Success
            $result['comments'] = ncn_chatter_render_post_comments($pid);   // Comments HTML
        }
    } else {
        $result['status'] = 'warning';
    }
    echo json_encode($result);
}

/**
 * chatter/lik-post-comment/%/%
 */
function ncn_chatter_like_post_comment($entity_id, $entity_type) {
global $user;
    $result = array(
        'status' => 'success', 
        'msg' => '', 
    );

    if (isset($_REQUEST['like_action'])) {
        $_action = $_REQUEST['like_action'];
        $action = 0;
        if ($_action == 'like') {
            $action = 1;
        }
        
        ncn_chatter_like_entity($user->uid, $entity_id, $entity_type, $action);

        if ($action) {
            $result['next_action'] = 'unlike';
        } else {
            $result['next_action'] = 'like';
        }
    } else {
        $result['status'] = 'error';
    }

    echo json_encode($result);
}

/*
 * URL: chatter/attach-file
 * 
 * Attach File
 */
function ncn_chatter_attach_file() {
    $result = array(
        'status' => 'success', 
        'msg' => '', 
    );

    $validators['file_validate_extensions'] = array();  // allow all extension
    $file = file_save_upload('chatter_file', $validators);
    if ($file) {
        $result['file'] = array(
            'fid'      => $file->fid, 
            'filename' => $file->filename, 
        );
    } else {
        $result['status'] = 'error';
        $result['msg'] = 'Failed to attach file. Please report this to administrator.';
    }

    echo json_encode($result);
}

function ncn_chatter_file_download_page($file) {
  // If the file does not exist it can cause problems with file_transfer().
  if (!is_file($file->uri)) {
    return MENU_NOT_FOUND;
  }
  // @todo Remove this once drupal_basename is fixed for PHP versions greater
  //  than '5.3.29'.
  $basename_function = version_compare(PHP_VERSION,'5.3.29','>') ? 'basename' : 'drupal_basename';
  $headers = array(
    'Content-Type' => mime_header_encode($file->filemime),
    'Content-Disposition' => 'attachment; filename="' . mime_header_encode($basename_function($file->uri)) . '"',
    'Content-Length' => $file->filesize,
    'Content-Transfer-Encoding' => 'binary',
    'Pragma' => 'no-cache',
    'Cache-Control' => 'must-revalidate, post-check=0, pre-check=0',
    'Expires' => '0',
  );

  // Let other modules alter the download headers.
  drupal_alter('file_download_headers', $headers, $file);

  if (ncn_chatter_file_is_local($file)) {
    // For local files, transfer the file and do not reveal the actual URL.
    file_transfer($file->uri, $headers);
  }
  else {
    // For remote files, just redirect the user to that file's actual URL.
    $headers['Location'] = file_create_url($file->uri);
    foreach ($headers as $name => $value) {
      drupal_add_http_header($name, $value);
    }
    drupal_send_headers();
    drupal_exit();
  }
}

function ncn_chatter_file_is_local($file) {
  $scheme = file_uri_scheme($file->uri);
  $wrappers = file_get_stream_wrappers(STREAM_WRAPPERS_LOCAL);
  return !empty($wrappers[$scheme]) && empty($wrappers[$scheme]['remote']);
}

function ncn_chatter_render_attached_file($fid) {
    if ($fid) {
        $file = file_load($fid);
        if ($file) {
            $output = '<div class="chatter-file-preview-section">';
            if (strpos($file->filemime, 'image')!==FALSE) {
                // Image 
                $file_url = $file->uri;
                
                $output.= '<a href="'.file_create_url($file_url).'" data-lity>';
                    $output.= theme('image_style', 
                                    array('style_name' => 'image_tab_upload', 
                                          'path' => $file_url));
                $output.= '</a>';
            } else {
                // Others
                $output.= ncn_chatter_file_entity_download_link(
                            array('file'=>$file,
                                  'text'=>$file->filename ));
            }
            $output.= '</div>';
            return $output;
        }
    }
    return '';
}

function ncn_chatter_file_entity_download_uri($file) {
    $uri = array('path' => "chatter/file/{$file->fid}/download", 'options' => array());
    return $uri;
}
function ncn_chatter_file_entity_download_link($variables) {
  $file = $variables['file'];
  $uri = ncn_chatter_file_entity_download_uri($file);

  // Human-readable names, for use as text-alternatives to icons.
  $mime_name = array(
    'application/msword' => t('Microsoft Office document icon'),
    'application/vnd.ms-excel' => t('Office spreadsheet icon'),
    'application/vnd.ms-powerpoint' => t('Office presentation icon'),
    'application/pdf' => t('PDF icon'),
    'video/quicktime' => t('Movie icon'),
    'audio/mpeg' => t('Audio icon'),
    'audio/wav' => t('Audio icon'),
    'image/jpeg' => t('Image icon'),
    'image/png' => t('Image icon'),
    'image/gif' => t('Image icon'),
    'application/zip' => t('Package icon'),
    'text/html' => t('HTML icon'),
    'text/plain' => t('Plain text icon'),
    'application/octet-stream' => t('Binary Data'),
  );

  $mimetype = file_get_mimetype($file->uri);

  $icon = theme('file_icon', array(
    'file' => $file,
    'icon_directory' => $variables['icon_directory'],
    'alt' => !empty($mime_name[$mimetype]) ? $mime_name[$mimetype] : t('File'),
  ));

  // Set options as per anchor format described at
  // http://microformats.org/wiki/file-format-examples
  $uri['options']['attributes']['type'] = $file->filemime . '; length=' . $file->filesize;

  // Provide the default link text.
  if (!isset($variables['text'])) {
    $variables['text'] = t('Download [file:name]');
  }

  // Perform unsanitized token replacement if $uri['options']['html'] is empty
  // since then l() will escape the link text.
  $variables['text'] = token_replace($variables['text'], array('file' => $file), array('clear' => TRUE, 'sanitize' => !empty($uri['options']['html'])));

  $output = '<span class="file">' . $icon . ' ' . l($variables['text'], $uri['path'], $uri['options']);
  $output .= ' ' . '<span class="file-size">(' . format_size($file->filesize) . ')</span>';
  $output .= '</span>';

  return $output;
}
