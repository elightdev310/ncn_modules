<form accept-charset="UTF-8" id="create-subuser-form" method="post" action="">
<div id="page_results">
    <div id="create-subuser" class="claims-detail">
        <div class="claims-section-wrapper">
            <div class="clearfix">
                <div class="title">Create Sub User</div>
                <div class="help"><a href="#">Help</a></div>
            </div>
            <div class="clearfix">
                <div id="create-subuser-wrapper">
                    <input type="hidden" name="tfunction" value="create-subuser">
                    <input type="hidden" name="parent_mid" value="<?php echo $user->profile_memberid; ?>">
                    <table class="form-table">
                        <tr>
                            <td class="tlabel"><label>Username:</label></td>
                            <td><input type="text" name="name" value="<?= $result['name']; ?>"></td>
                        </tr>
                        <!-- parent member id -->
                        <tr>
                            <td class="tlabel"><label>E-mail:</label></td>
                            <td><input type="text" name="mail" value="<?= $result['mail']; ?>" AUTOCOMPLETE="off"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Password:</label></td>
                            <td><input type="password" name="pass" value="<?= $result['pass']; ?>" AUTOCOMPLETE="off"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Confirm Password:</label></td>
                            <td><input type="password" name="pass2" value="<?= $result['pass2']; ?>" AUTOCOMPLETE="off"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>First Name:</label></td>
                            <td><input type="text" name="profile_firstname" value="<?= $result['profile_firstname']; ?>"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Last Name:</label></td>
                            <td><input type="text" name="profile_lastname" value="<?= $result['profile_lastname']; ?>"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Company's Name:</label></td>
                            <td><input type="text" name="profile_legalname" value="<?= $result['profile_legalname']; ?>"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Country:</label></td>
                            <td><?php draw_countryform_part_sel('profile_country', false, false,$result['profile_country']); ?></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Address:</label></td>
                            <td><input type="text" name="profile_address" value="<?= $result['profile_address']; ?>"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>City:</label></td>
                            <td><input type="text" name="profile_city" value="<?= $result['profile_city']; ?>"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Province/State:</label></td>
                            <td><?php draw_stateform_part_sel('profile_state', false, false,$result['profile_country'],$result['profile_state']); ?></td>
                        </tr>
                        <tr>
                            <td class="tlabel"><label>Zip:</label></td>
                            <td><input type="text" name="profile_zip" value="<?= $result['profile_zip']; ?>"></td>
                        </tr>
                        <tr>
                            <td class="tlabel"></td>
                            <td><input type="submit" class="form-submit subuser-form-btn" value="Save" name="op" id="edit-submit"></td>
                        </tr>
                    </table>
                </div>
            </div>
        </div>
    </div>
</div>
</form>