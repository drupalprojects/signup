<?php
// $Id$


/**
 * @file
 * Code related to the signup administration tab on each node.
 */

/**
 * Print the signup administration tab for a single node.
 */
function signup_node_admin_page($node) {
  drupal_set_title(check_plain($node->title));

  // Administrative table to control signups for this node.
  module_load_include('inc', 'signup', 'includes/node_admin_summary');
  $signup_node_admin_summary_form = drupal_get_form('signup_node_admin_summary_form', $node);

  // Signup details table, including cancel checkboxes.
  $signup_node_admin_details_form = drupal_get_form('signup_node_admin_details_form', $node);

  if ($node->signup_status) {
    // Add a form to allow the administrator to signup other users.
    module_load_include('inc', 'signup', 'includes/signup_form');
    $signup_form = drupal_get_form('signup_form', $node, 'admin');
  }
  else {
    $signup_form = '';
  }

  return theme('signup_node_admin_page', $node, $signup_node_admin_summary_form, $signup_node_admin_details_form, $signup_form);
}

function signup_node_admin_details_form(&$form_state, $node) {
  unset($_SESSION['signup_cancel_multiple_users']);
  $form = array();

  // Prepare a table header that allows sorting on name and signup time.
  $header = array(
    theme('table_select_header_cell'),
    array('data' => t('Name'), 'field' => 'u.name', 'sort' => 'asc'),
    array('data' => t('Signup time'), 'field' => 's.signup_time'),
    array('data' => t('Extra information')),
  );

  $sql = "SELECT u.uid, u.name, s.* FROM {signup_log} s INNER JOIN {users} u ON u.uid = s.uid WHERE s.nid = %d";
  $sql .= tablesort_sql($header);
  $result = db_query($sql, $node->nid);

  // Loop through the users, unserializing their user data.
  while ($signed_up_user = db_fetch_object($result)) {
    // The "username" to print is different for anon signups and registered
    // user signups.  For registered users, provide a link to the user profile
    // For anon, use the user's email address as the name.
    if ($signed_up_user->uid == 0) {
      $username = check_plain($signed_up_user->anon_mail);
    }
    else {
      $username = theme('username', $signed_up_user);
    }
    $key = $signed_up_user->sid;
    $users[$key] = '';
    $form['username'][$key] = array('#value' => $username);
    $form['signup_date'][$key] = array('#value' => format_date($signed_up_user->signup_time, variable_get('signup_date_format', 'small')));
    $form['signup_form_data'][$key] = array('#value' => theme('signup_custom_data', unserialize($signed_up_user->form_data)));
  }
  if (empty($users)) {
    $form['no_users'] = array('#value' => t('No users have signed up for this %node_type.', array('%node_type' => node_get_types('name', $node->type))));
  }
  else {
    $form['nid'] = array(
      '#type' => 'hidden',
      '#value' => $node->nid,
    );
    $form['users'] = array('#type' => 'checkboxes', '#options' => $users);
    $form['submit_cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel signups'),
      '#submit' => array('signup_cancel_multiple_submit'),
      '#validate' => array('signup_cancel_multiple_validate'),
    );
    $form['#header'] = $header;
  }
  return $form;
}

function signup_cancel_multiple_validate($form, &$form_state) {
  $users = array_filter($form_state['values']['users']);
  if (empty($users)) {
    form_set_error('', t('No users selected.'));
  }
}

/**
 * Submit handler for cancelling multiple signups via node/N/signups.
 *
 * This saves the selected users into SESSION and redirects to a confirm form
 * which is registered at node/N/signups/confirm.
 */
function signup_cancel_multiple_submit($form, &$form_state) {
  $_SESSION['signup_cancel_multiple_users'] = array_filter($form_state['values']['users']);
  $form_state['redirect'] = 'node/'. $form_state['values']['nid'] .'/signups/confirm';
}

/**
 * Builds the confirm form when canceling multiple signups from node/N/signups.
 */
function signup_cancel_multiple_confirm(&$form_state, $node) {
  $form = array();
  $form['nid'] =  array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );
  $form['users'] =  array(
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
    '#tree' => TRUE,
  );
  $placeholders = db_placeholders($_SESSION['signup_cancel_multiple_users']);
  $query = db_query("SELECT u.name, u.uid, s.* FROM {signup_log} s INNER JOIN {users} u ON s.uid = u.uid WHERE s.sid IN (". $placeholders .")", $_SESSION['signup_cancel_multiple_users']);
  while ($signup = db_fetch_object($query)) {
    $key = $signup->sid;
    if ($signup->uid) {
      $label = theme('username', $signup);
    }
    else {
      $label = t('Anonymous signup: %anon_mail', array('%anon_mail' => $signup->anon_mail));
    }
    $form['users'][$key] = array(
      '#type' => 'hidden',
      '#value' => $key,
      '#prefix' => '<li>',
      '#suffix' => $label ."</li>\n",
    );
  }
  $form['#submit'][] = 'signup_cancel_multiple_confirm_submit';
  return confirm_form(
    $form,
    t('Are you sure you want to cancel signups for these users?'),
    'node/'. $node->nid .'/signups',
    t('This action cannot be undone.'),
    t('Cancel signups'), t('Keep signups')
  );
}

/**
 * Submit handler for the confirm form to cancel multiple signups.
 */
function signup_cancel_multiple_confirm_submit($form, &$form_state) {
  $nid = $form_state['values']['nid'];
  foreach ($form_state['values']['users'] as $key) {
    signup_cancel_signup($key);
  }
  $form_state['redirect'] = 'node/'. $nid .'/signups';
  unset($_SESSION['signup_cancel_multiple_users']);
}

