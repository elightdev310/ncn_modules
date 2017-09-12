<!--[if IE]>

<style>
.tabs_inline_inner	{
	min-width:3024px;
}

#roomtabs	{
	/*height:48px;*/
}
</style>

<![endif]-->
<div id="inline_wrapper">
    <div id="inline_content_wrapper">

        <div style="width:816px;white-space:nowrap;position:relative;">
            <div class="tabs_inline_inner">
                <ul class="tabs clear-block" id="roomtabs" style="width:813px;">
                    <?php
                    $c = 1;
                    $tab_index = 1;
                    if ($room_choose_type == 'tab') { $tab_index = $room_choose_value; }
                    foreach ($_SESSION['claim'][$claim_id]['scope'] as $roomname => $roomdata)
                    {
                        if ($room_choose_type == 'room') {
                            if ($roomname == $room_choose_value) {
                                $tab_index = $c;
                            }
                        }
                        ?>

                        <li id="tab<?= ($c) ?>">
                            <?php if ($c > 1): ?>
                                <div class="delete_room_button" onclick="delete_room(<?= $c; ?>, <?= $claim_id ?>, '<?= $roomname ?>');"></div>
                            <?php endif; ?>

                            <a href="<?= base_path() ?>account/roomtab/<?= $claim_id ?>/<?= $roomname ?>/<?= ($c); ?>" onclick="return tab_click(<?= ($c); ?>);" class="roomname" id="roomname_text_<?= $c; ?>"><?= $roomname; ?></a>

                            <?php if ($c > 1): ?>
                                <a href="#" onclick="rename_room(<?= $c; ?>, <?= $claim_id ?>, '<?= $roomname ?>');" class="roomedit" id="rename_roomname_text_<?= $c; ?>">[edit]</a>
                            <?php endif; ?>
                        </li>

                        <?php
                        $c++;
                    }
                    ?>
                    <li id="add_room_tab"><a href="#" onclick="add_room(<?= $claim_id ?>);return false;">Add Room</a></li>
                </ul>
            </div>
        </div>

        <script type="text/javascript">
            var tabindex = <?= ($c-1); ?>;
        </script>

        <div class="tab_container">
            <div id="tabcontent">
            </div>

            <div id="tabloading">
            </div>

        </div>
        <div style="clear: both"></div>
        <form method="POST" id="save_photos_form">
            <input type="hidden" name="tfunction" value="process">
            <tr>
                <td>&nbsp;</td>

                <?php
                if (!isset($GLOBALS['in_edit']))
                {
                    ?>
                    <td><input type="submit" value="Submit"> or <input type="button" value="Save and Finish Later" onclick="window.location='6';return false;"> or <a href="<?= $GLOBALS['base_url']; ?>/account">cancel</a></td>
                <?php
                }
                else
                {
                    ?>
                    <td><input type="button" value="Save" id="save_jquery" onclick="save_photos_inline('<?= $_SERVER['REQUEST_URI'] ?>');"></td>
                    <td>
                        <!--                <input type="submit" name="edit_posted" value="Save and Close">-->
                        <input type="button" value="Save and Close" id="save_jquery_and_close" onclick="save_photos_inline_and_close('<?= $_SERVER['REQUEST_URI'] ?>');">
                    </td>

                <?php
                }
                ?>
            </tr>

        </form>

        <script>
            jQuery(document).ready(function() {
                setTimeout(function(){
                    <?php if ($c > 1): ?>
                    //When page loads...
                    jQuery(".tab_content").hide(); //Hide all content
                    jQuery("ul.tabs li:first").addClass("active").show(); //Activate first tab
                    jQuery(".tab_content:first").show(); //Show first tab content
                    <?php if ($tab_index>$c) { $tab_index = 1; } ?>
                    tab_click(<?php print $tab_index; ?>);  // load default page
                    <?php else: ?>
                    jQuery("#tabloading").css('display', 'none'); //Hide all content
                    <?php endif; ?>
                }, 500);
            });
        </script>
    </div>
</div>
