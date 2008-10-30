<?php
// $Id$

/**
 * Define the default views provided by the signup module.
 *
 * @see signup_views_default_views()
 */
function _signup_views_default_views() {

  // Signup schedule at user/N/signups and the "Current signups" block.
  $view = new stdClass();
  $view->name = 'signup_current_signups';
  $view->description = t('A list of all signups for the current user');
  $view->access = array();
  $view->view_args_php = 'if ($type == \'block\') {
  global $user;
  return array(0 => $user->uid);
}
';
  $view->page = TRUE;
  $view->page_title = t('Current signups');
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = t('This user has not signed up for any content.');
  $view->page_empty_format = '1';
  $view->page_type = 'list';
  $view->url = 'user/$arg/signups/current';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '20';
  $view->menu = TRUE;
  $view->menu_title = t('Current');
  $view->menu_tab = TRUE;
  $view->menu_tab_weight = '-2';
  $view->menu_tab_default = TRUE;
  $view->menu_tab_default_parent_type = 'tab';
  $view->menu_parent_tab_weight = '10';
  $view->menu_parent_title = t('Signups');
  $view->block = TRUE;
  $view->block_title = t('Current signups');
  $view->block_header = '';
  $view->block_header_format = '1';
  $view->block_footer = '<?php
global $user;
print l(t("View signup schedule"), "user/$user->uid/signups");
?>
';
  $view->block_footer_format = '2';
  $view->block_empty = '';
  $view->block_empty_format = '1';
  $view->block_type = 'list';
  $view->nodes_per_block = '10';
  $view->block_more = TRUE;
  $view->block_use_page_header = FALSE;
  $view->block_use_page_footer = FALSE;
  $view->block_use_page_empty = FALSE;
  $view->sort = array();
  $view->argument = array(
    array(
      'type' => 'signup_uid',
      'argdefault' => '1',
      'title' => '',
      'options' => '1',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array(
    array(
      'tablename' => 'node',
      'field' => 'title',
      'label' => '',
      'handler' => 'views_handler_field_nodelink_with_mark',
      'sortable' => '1',
      'options' => 'link',
    ),
  );
  $view->filter = array(
    array (
      'tablename' => 'node',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
    array(
      'tablename' => 'signup',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
  );
  $view->exposed_filter = array();
  $view->requires = array(node, signup);
  $views[$view->name] = $view;

  // Subtab of user/N/signups for all signup-enabled nodes that the
  // user hasn't replied to and the "Available signups" block.
  $view = new stdClass();
  $view->name = 'signup_available_signups';
  $view->description = t('A view of all open signups the user has not replied to.');
  $view->access = array();
  $view->view_args_php = 'if ($type == \'block\') {
  global $user;
  return array(0 => $user->uid);
}
';
  $view->page = TRUE;
  $view->page_title = t('Available signups');
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = t('Congratulations, you have replied to all of the content on the site that is accepting signups.');
  $view->page_empty_format = '1';
  $view->page_type = 'list';
  $view->url = 'user/$arg/signups/available';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '20';
  $view->menu = TRUE;
  $view->menu_title = t('Available');
  $view->menu_tab = TRUE;
  $view->menu_tab_weight = '10';
  $view->menu_tab_default = FALSE;
  $view->menu_tab_default_parent = NULL;
  $view->menu_parent_tab_weight = '0';
  $view->menu_parent_title = '';
  $view->block = TRUE;
  $view->block_title = t('Available signups');
  $view->block_header = '';
  $view->block_header_format = '1';
  $view->block_footer = '';
  $view->block_footer_format = '1';
  $view->block_empty = '';
  $view->block_empty_format = '1';
  $view->block_type = 'list';
  $view->nodes_per_block = '10';
  $view->block_more = TRUE;
  $view->block_use_page_header = FALSE;
  $view->block_use_page_footer = FALSE;
  $view->block_use_page_empty = FALSE;
  $view->sort = array();
  $view->argument = array(
    array(
      'type' => 'signup_uid',
      'argdefault' => '1',
      'title' => '',
      'options' => '0',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array(
    array(
      'tablename' => 'node',
      'field' => 'title',
      'label' => '',
      'handler' => 'views_handler_field_nodelink_with_mark',
      'sortable' => '1',
      'options' => 'link',
    ),
  );
  $view->filter = array(
    array (
      'tablename' => 'node',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
    array(
      'tablename' => 'signup',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
  );
  $view->exposed_filter = array();
  $view->requires = array(node, signup);
  $views[$view->name] = $view;

  $view = new stdClass();
  $view->name = 'signup_user_list';
  $view->description = 'List all users signed up for a node';
  $view->disabled = TRUE;
  $view->access = array ();
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = '';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = 'node/$signup/signup-list';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '200';
  $view->menu = TRUE;
  $view->menu_title = 'Signup list';
  $view->menu_tab = TRUE;
  $view->menu_tab_weight = '0';
  $view->menu_tab_default = FALSE;
  $view->menu_tab_default_parent = NULL;
  $view->menu_tab_default_parent_type = 'tab';
  $view->menu_parent_tab_weight = '0';
  $view->menu_parent_title = '';
  $view->sort = array ();
  $view->argument = array (
    array (
      'type' => 'nid',
      'argdefault' => '1',
      'title' => '%1',
      'options' => '0',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array (
    array (
      'tablename' => 'signup_log',
      'field' => 'uid',
      'label' => 'Username',
      'sortable' => '1',
    ),
    array (
      'tablename' => 'signup_log',
      'field' => 'signup_time',
      'label' => 'Time signed up',
      'handler' => 'views_handler_field_since',
      'sortable' => '1',
      'defaultsort' => 'DESC',
    ),
  );
  $view->filter = array ();
  $view->exposed_filter = array ();
  $view->requires = array(signup_log);
  $views[$view->name] = $view;

  return $views;
}
