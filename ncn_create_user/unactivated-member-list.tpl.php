<?php

    global $base_url;

    if (arg(4) == 'member_delete')
    {
        ncn_admin_unactivated_member_delete(arg(5));
    }

    drupal_add_js(drupal_get_path('module', 'ncn_admin') . '/ncn_admin.js');
    $header = array(
        array(
            'data' => t('Member ID')
        ) ,
        array(
            'data' => t('Name')
        ) ,
        array(
            'data' => t('Status')
        ) ,
        array(
            'data' => t('Company')
        ) ,
        array(
            'data' => t('Type')
        ) ,
        array(
            'data' => t('Contact Email')
        ) ,
        array(
            'data' => t('SignUp Date')
        ) ,
        array(
            'data' => t('Owner')
        ) ,
        array(
            'data' => t('Operation')
        )
    );

    // var_dump($base_url);
    // exit;
    //    $query = "SELECT * FROM member_id_pool WHERE used=0 ORDER BY id DESC";
    //   $result = pager_query($query, 50);

    $result = db_select('member_id_pool', 'n')->fields('n')->condition('used', 0, '=')->orderBy('id', 'DESC')->execute();

    //        ->extend('TableSort');
    // $result = $select->extend('PagerDefault')->limit(50);
    //    echo $result;
    //    exit;
    //   while ($row = db_fetch_array($result))

    foreach($result as $row)
    {
        $row = (array)$row;
        $member_type = "Gold";
        if ($row['member_type'] == 0)
        {
            $member_type = "Gold";
        }
        else
            if ($row['member_type'] == 1)
            {
                $member_type = "Silver";
            }
            else
                if ($row['member_type'] == 2)
                {
                    $member_type = "Gold Lite";
                }
                else
                    if ($row['member_type'] == 3)
                    {
                        $member_type = "Coach on Call";
                    }
                    else
                        if ($row['member_type'] == 4)
                        {
                            $member_type = "Gold Coach";
                        }
                        else
                            if ($row['member_type'] == 5)
                            {
                                $member_type = "CSI Member";
                            }

        if (ncn_admin_is_gold_member_demo($row['member_id']))
        {
            $member_type.= " (Demo)";
        }
        else
            if (ncn_admin_is_member_first_free($row['member_id']))
            {
                $member_type.= " (First Free)";
            }

        // is account free?

        $free_extra = '';
        if (is_member($row['member_id']))
        {
            $result = db_query("SELECT customerProfileId FROM {member_cim} WHERE member_id=:s", array(
                ":s" => $row['member_id']
            ))->fetchField();

            //    var_dump($result);
            //    exit;

            if (intval($result) == 0)
            {
                $free_extra = '(FREE)';
            }
        }

        $status = 'unactivated';
        if ($row['status'] == 0)
        {
            $status = 'paused';
        }

        $_owner = user_load($row['owner']);

        //  var_dump($_owner);
        // exit;

        $rows[] = array(
            'data' => array(

                // Cells

                $row['member_id'],
                $row['first_name'] . ' ' . $row['last_name'] . $free_extra,
                $status,
                $row['legalname'],
                $member_type,
                $row['contactemail'],
                date('d M Y', $row['create']) ,
                $_owner->profile_firstname . ' ' . $_owner->profile_lastname,
                "<a href='$base_url/admin/config/ncn_create_user/unactivated_member_list/edit/" . $row['member_id'] . "'>Edit</a>&nbsp;&nbsp;" .

                // "<a href='$base_url/admin/user/user/unactivated_member_list/member_delete/".$row['member_id']."' onclick='return on_click_ncn_admin_una_member_delete();'>Delete</a>&nbsp;&nbsp;"

                "<a href='$base_url/admin/config/ncn_create_user/unactivated_member_list/member_delete/" . $row['member_id'] . "' onclick='return on_click_ncn_admin_una_member_delete();'>Delete</a>&nbsp;&nbsp;"
            ) ,
        );
    }

    if (!$row)
    {
        $rows[] = array(
            array(
                'data' => t("There isn't unactivated member.") ,
                'colspan' => 9
            )
        );
    }

    $output = '';
    $output.= theme('table', array(
        "header" => $header,
        "rows" => $rows,
        'id' => 'una_member_list'
    ));
    $output.= theme('pager');
    echo $output;
?>
