<?php
// $Id$

/**
 * @return Array of SQL clauses for cron reminder email query builder.
 */
function signup_reminder_sql($content_type_field = NULL) {
  global $db_type;
  switch ($db_type) {
    case 'mysql':
    case 'mysqli':
      $where = array("NOW() + INTERVAL s.reminder_days_before DAY > ". date_sql('DATE', $content_type_field['database']['columns']['value']['column'], 'iso'));
      break;
    case 'pgsql':
      $where = array("NOW() + INTERVAL s.reminder_days_before days > ". date_sql('DATE', $content_type_field['database']['columns']['value']['column'], 'iso'));
      break;
  }
  return array(
    'fields' => array('n.type', $content_type_field['database']['columns']['value']['column']),
    'joins' => array('LEFT JOIN {'. $content_type_field['database']['table'] .'}  ON {'. $content_type_field['database']['table'] .'}.vid = n.vid'),
    'where' => $where,
  );
}

/**
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function signup_autoclose_sql($content_type_field = NULL) {
  return array(
    'fields' => array($content_type_field['database']['columns']['value']['column']),
    'joins' => array(
      'INNER JOIN {node} n ON s.nid = n.nid',
      'LEFT JOIN {'. $content_type_field['database']['table'] .'} ON {'. $content_type_field['database']['table'] .'}.vid = n.vid',
    ),    
    'where' => array( date_sql('DATE', $content_type_field['database']['columns']['value']['column'], 'iso') ." < '". date('Y-m-d H:i:s', time() + variable_get('signup_close_early', 1) * 3600) ."'"),
  );
}

/**
 * @return SQL for listing signups a user has signed up for
 */
function signup_list_user_signups_sql($content_type = NULL) {
  $field = signup_date_field($content_type);
  if ($field && $field != 'none') {
    return 'SELECT n.nid, n.title, '. $field['database']['columns']['value']['column'] .' FROM {node} n INNER JOIN {signup_log} s_l ON n.nid = s_l.nid LEFT JOIN '. $field['database']['table'] .' c ON c.nid = n.nid WHERE s_l.uid = %d AND ('. date_sql('NOW', $field['database']['columns']['value']['column'], 'iso') .' >= NOW() OR '. $field['database']['columns']['value']['column'] .' IS NULL) ORDER BY '. $field['database']['columns']['value']['column'];
  } 
  else {
    return FALSE;
  }
}

/**
 * Returns TRUE if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_event_completed($node) {
  if (is_numeric($node)) {
    $node = node_load($node);
  }
  $field = signup_date_field($node->type);
  if ($field && $field != 'none' && isset($node->{$field['field_name']})) {
    $closing_time = time() + (variable_get('signup_close_early', 1) * 3600);
    if (date_iso2unix($node->{$field['field_name']}[0]['value']) < $closing_time) {
      return TRUE;
    }
  }
  return FALSE;
}

function signup_format_date($node, $include_to_date = FALSE) {
  $field = signup_date_field($node->type);
  if (!$field || $field == 'none') {
    return '';
  }
  if (isset($node->{$field['field_name']})) {
    $date_value = $node->{$field['field_name']}[0]['value'];
  }
  else if (isset($node->{$field['field_name'] . '_value'})) {
    $date_value = $node->{$field['field_name'] . '_value'};
  }
  else {
    $date_value = $node->{$field['database']['columns']['value']['column']};
  }
  $date = date_show_date(date_make_date($date_value), $field['output_format_date'], 'db');

  if ($include_to_date) {
    if (isset($node->{$field['field_name']})) {
      $date_value = $node->{$field['field_name']}[0]['value2'];
    }
    else {
      $date_value = $node->{$field['database']['columns']['value2']['column']};
    }
    if ($date_value) {
      $date .= t(' to ') . date_show_date(date_make_date($date_value), $field['output_format_date'], 'db');
    }
  }

  return $date;
}
