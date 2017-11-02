<?php

/**
 * @file
 * This file is Feed Post Form template
 *
 * @var $feed_id
 */
global $user;
$is_ncn_user = is_ncn_user($user);
$action_url = url('chatter/add/post/'.$feed_id);
?>


<form id="ncn_chatter_post_form" action="<?php echo $action_url ?>" class="" enctype="multipart/form-data" method="POST">
    <input type="hidden" name="tfunction" value="chatter_create_post" />
    <div class="panel-box">
    <div class="post-field">
        <div class="post-box">
            <div class="author-photo text-label"><span>POST</span></div>
            <div class="mentions-container">
                <div class="mentions-input">
                    <textarea class="status-input-text" name="content" row="1" placeholder="Share and update..."></textarea>
                </div>
                <div class="clearfix">
                    <div class="post-option-container">
                        <?php if ($is_ncn_user): ?>
                        <select class="post-filter" name="post_filter">
                            <option value="1">Netclaimsnow Only</option>
                            <option value="0">Public</option>
                        </select>
                        <?php endif; ?>
                    </div>
                    <div class="button-container">
                        <button type="button" class="btn btn-flat add-post-btn">Share</button>
                    </div>
                </div>
            </div>
        </div>
    </div>
    </div>
</form>

<script>
jQuery(function($) {
    $(document).ready(function() {
        $('.chatter-section').on('keyup', 'textarea.status-input-text', function() {
            textAreaAdjust(this);
        });
        

        // Add Post
        $('.chatter-section .add-post-btn').on('click', function() {
            var $_form = $('#ncn_chatter_post_form');
            if (!$_form.find('textarea.status-input-text').val()) {
                return false;
            }
            var _url  = $_form.attr('action');
            $.ajax({
                url:    _url,
                type:   'POST',
                data:   $_form.serialize(), 
                beforeSend: function(jqXHR, settings) {},
                error: function() {},
                success: function(response) {
                    eval("var json=" + response + ";");
                    if (json.status == "success") {
                        $('#ncn_chatter_post_form textarea.status-input-text').val('');
                        ncn_feed_post_next_page(0, 'start');
                    } else {
                        if (json.msg != "") { alert(json.msg); }
                    }
                }, 
                complete: function(jqXHR, textStatus) {}
            }); 
        });
        // Add Comment
        $('.chatter-section').on('click', '.add-comment-btn', function() {
            var $_this = $(this);
            var $_comment_section = $_this.closest('.comment-section');
            var $_form = $_this.closest('form.ncn_chatter_comment_form');
            if (!$_form.find('textarea.status-input-text').val()) {
                return false;
            }
            var _url  = $_form.attr('action');
            $.ajax({
                url:    _url,
                type:   'POST',
                data:   $_form.serialize(), 
                beforeSend: function(jqXHR, settings) {},
                error: function() {},
                success: function(response) {
                    eval("var json=" + response + ";");
                    if (json.status == "success") {
                        $_form.find('textarea.status-input-text').val('');
                        // Render Comments
                        $_comment_section.find('.comment-list').html(json.comments);
                    } else {
                        if (json.msg != "") { alert(json.msg); }
                    }
                }, 
                complete: function(jqXHR, textStatus) {}
            }); 
        });

    });
});
</script>
