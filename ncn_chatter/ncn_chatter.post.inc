<?php

/**
 * Insert record to chatter_post table
 */
function ncn_chatter_insert_post_record($data=array()) {
    $result = db_insert('chatter_post')
            ->fields(array(
                'content'   => $data['content'], 
                'ncn_only'  => (isset($data['ncn_only'])? $data['ncn_only'] : 0), 
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
 * @param   $data:array( content, ncn_only: optional )
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
 * @param   $data:array( content, pid )
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
    $ncn_only= (isset($_REQUEST['post_filter']))? $_REQUEST['post_filter']:0;

    if ($content) {
        $pid = ncn_chatter_create_post($user->uid, $fid, array(
            'content' => $_REQUEST['content'], 
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
    if ($content) {
        $cid = ncn_chatter_create_comment($user->uid, array(
            'pid'     => $pid, 
            'content' => $_REQUEST['content']
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
