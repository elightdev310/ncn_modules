<?php

/**
 * @file
 * This file is Post Comment template
 *
 * @var $comment
 */
$hidden_style = (isset($comment['hidden'])&&$comment['hidden'])? 'hidden-comment':'';
?>

<div class="comment-box <?php echo $hidden_style ?>">
    <div class="author-photo">
        <div style="width:32px; height: 32px; background: blue;">&nbsp;</div>
    </div>
    <div class="mentions-container">
        <div class="comment-meta-info">
            <div>
                <strong><?php echo ncn_chatter_render_user_name($comment['uid']) ?></strong>
            </div>
            <div class="created-at">
                <?php echo date("F j, Y \a\\t g:i A", $comment['created_at']) ?>
            </div>
        </div>
        <div class="comment-content">
            <?php echo $comment['content'] ?>
        </div>
        <div class="comment-action-links">
            <a href="#" class="post-like-link post-action-link">Like</a>
        </div>
    </div>
</div>
