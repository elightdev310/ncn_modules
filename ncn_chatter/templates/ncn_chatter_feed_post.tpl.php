<?php

/**
 * @file
 * This file is Feed Post template
 *
 * @var $post
 */
global $user;
$post_like_class = '';
if (ncn_chatter_check_user_like_entity($user->uid, $post['id'], 0)) {
    $post_like_class = 'unlike';
} else {
    $post_like_class = 'like';
}
$post_like_url = url('chatter/like-post-comment/'.$post['id'].'/0');
$action_url = url('chatter/post/'.$post['id'].'/add/comment');
?>

<div class="panel-box post-panel-box">
    <div class="post-field">
        <div class="post-box">
            <div class="author-photo">
                <div style="width:32px; height: 32px; background: blue;">&nbsp;</div>
            </div>
            <div class="mentions-container">
                <div class="post-meta-info">
                    <div>
                        <strong><?php echo ncn_chatter_render_user_name($post['uid']) ?></strong>
                    </div>
                    <div class="created-at">
                        <?php echo date("F j, Y \a\\t g:i A", $post['created_at']) ?>
                    </div>
                </div>
                <div class="post-content">
                    <?php echo $post['content'] ?>
                </div>
                <div class="post-action-links">
                    <?php if ($user->uid != $post['uid']): ?>
                    <a href ="<?php echo $post_like_url ?>" 
                       class="post-like-link post-action-link <?php echo $post_like_class ?>">
                        <?php echo ucwords($post_like_class) ?> 
                    </a>
                    <?php endif; ?>
                    
                    <a href="#" class="post-comment-link post-action-link">Comment</a>
                </div>
                <div class="comment-section">
                    <div class="comment-list">
                        <?php echo ncn_chatter_render_post_comments($post['id']) ?>
                    </div>

                    <form class="ncn_chatter_comment_form <?php echo (isset($post['count_comments'])&&$post['count_comments']==0)? 'hidden-form':'' ?>" 
                          action="<?php echo $action_url ?>" class="" enctype="multipart/form-data" method="POST">
                        <input type="hidden" name="tfunction" value="chatter_create_comment" />
                        <div class="comment-box">
                            <div class="author-photo">
                                <div style="width:32px; height: 32px; background: blue;">&nbsp;</div>
                            </div>
                            <div class="mentions-container">
                                <div class="mentions-input">
                                    <textarea class="status-input-text" name="content" row="1" placeholder="Write a comment..."></textarea>                    
                                </div>
                                <div class="clearfix">
                                    <div class="button-container">
                                        <button type="button" class="btn btn-flat add-comment-btn">Write</button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </form>

                </div>
            </div>
        </div>
    </div>
</div>
