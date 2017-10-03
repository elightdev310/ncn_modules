<?php
define('NCN_TEST', 'TEST');

// User Role
define('USER_ROLE_AUTHENTICATE',                     2);
define('USER_ROLE_EDITOR_USER',                      3);
define('USER_ROLE_DISTRIBUTOR',                      5);
define('USER_ROLE_ACCOUNT_MANAGER',                  6);
define('USER_ROLE_ASSOCIATE',                        7);
define('USER_ROLE_MEMBER',                           8);
define('USER_ROLE_DIRECTOR_OF_MEMBER_SERVICE',       9);
define('USER_ROLE_EXECUTIVE',                       11);
define('USER_ROLE_SUB_USER',                        12);
define('USER_ROLE_CLAIM_EXAMINER',                  26);    // Deleted : RID = 14
define('USER_ROLE_SALES_DIRECTOR',                  15);
define('USER_ROLE_SENIOR_ACCOUNT_MANAGER',          17);
define('USER_ROLE_LEADUSER',                        23);
define('USER_ROLE_BLOCKED_DELETE',                  24);
define('USER_ROLE_ADMIN',                           25);

// Member Type
define('MT_GOLD',                                    0);
define('MT_SILVER',                                  1);
define('MT_GOLD_LITE',                               2);
define('MT_COACH_ON_CALL',                           3);
define('MT_GOLD_COACH',                              4);
define('MT_CSI',                                     5);

// Product - Retail Kit
define('SKU_SUF_DF',                                33);    // $279, $329
define('SKU_RETAIL_KIT',                            34);    // $329
define('SKU_CSI_SIGN_UP',                           40);    // $0 
define('SKU_NCN_RETAIL_KIT_OPT',                    41);    // $149
define('SKU_NCN_PREMIUM_KIT_OPT',                   42);    // $249
define('SKU_SUF_DP',                                43);    // $99, $149
define('SKU_SUF_GMS',                               47);    // $99
define('SKU_SUF_GMSD',                              48);    // $49, $99
define('SKU_SUF_GMS_1ST',                           57);    // $0
define('SKU_SUF_G_DEMO',                            68);    // $0
define('SKU_SUF_GMAS',                              88);    // $49
define('SKU_SUF_GC',                               154);    // $153.90
define('SKU_SUF_COC',                              155);    // $54.90


// Product - Recurring Fee
define('SKU_MMF',                                   32);    // $99
define('SKU_MMF_GL',                               102);    // $24.95
define('SKU_MMF_COC',                              119);    // $39.95
define('SKU_MMF_GC',                               137);    // $138.95

//
define('JAR_JODCONVERTER', '/jod/lib/jodconverter-core-3.0-beta-4.jar');


// CapsuleCRM Custom Field
define('CRM_FIELD_MEMBER_ID',                   441142);    // Member ID #
define('CRM_FIELD_DIST_ASSO',                   441179);    // Distributor / Associate
define('CRM_FIELD_ACCT_MNGR',                   441181);    // Account Manager
define('CRM_FIELD_USERNAME' ,                   441182);    // Username
define('CRM_FIELD_ORG_PWD'  ,                   441183);    // Original Password
define('CRM_FIELD_DATE_PURCHASE' ,              441202);    // Date of Purchase
define('CRM_FIELD_SALES_REP',                   441192);    // Sales Rep

define('CRM_FIELD_COUNT_CLAIMS',                441285);    // # of Claims
define('CRM_FIELD_TOTAL_YTD',                   441286);    // Total $$ YTD
define('CRM_FIELD_TOTAL_ALL_TIME',              441287);    // Total $$ All Time 

define('CRM_FIELD_VOS_ACCOUNT',                 441288);    // VOS account
