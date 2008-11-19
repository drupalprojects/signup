<?php
// $Id$


/**
 * @file
 * Code required to support version 5.x-2.* of the CCK date field module.
 */

/**
 *
 * @return Array of SQL clauses for cron reminder email query builder.
 */
function _signup_date_reminder_sql($content_type) {
  $field = signup_date_field($content_type);
  global $db_type;
  require_once drupal_get_path('module', 'date_api') .'/date_api_sql.inc';
  $handler = new date_sql_handler();
  switch ($db_type) {
    case 'mysql':
    case 'mysqli':
      $where = array(
        "NOW() + INTERVAL s.reminder_days_before DAY > ". $handler->sql_extract('DATE', $field['database']['columns']['value']['column']),
        "NOW() <= ". $handler->sql_extract('DATE', $field['database']['columns']['value']['column']),
      );
      break;

    case 'pgsql':
      $where = array(
        "NOW() + INTERVAL s.reminder_days_before days > ". $handler->sql_extract('DATE', $field['database']['columns']['value']['column']),
        "NOW() <= ". $handler->sql_extract('DATE', $field['database']['columns']['value']['column']),
      );
      break;
  }
  return array(
    'fields' => array($field['database']['columns']['value']['column']),
    'joins' => array('LEFT JOIN {'. $field['database']['table'] .'} ON {'. $field['database']['table'] .'}.vid = n.vid'),
    'where' => $where,
  );
}

/**
 *
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function _signup_date_autoclose_sql($content_type) {
  $field = signup_date_field($content_type);
  require_once drupal_get_path('module', 'date_api') .'/date_api_sql.inc';
  $handler = new date_sql_handler();
  return array(
    'fields' => array($field['database']['columns']['value']['column']),
    'joins' => array(
      'LEFT JOIN {'. $field['database']['table'] .'} ON {'. $field['database']['table'] .'}.vid = n.vid',
    ),
    'where' => array($handler->sql_extract('DATE', $field['database']['columns']['value']['column']) ." < '". date('Y-m-d\TH:i:s', time() + variable_get('signup_close_early', 1) * 3600) ."'"),
  );
}

/**
 * Returns TRUE if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_date_node_completed($node) {
  $field = signup_date_field($node->type);
  if ($field && $field != 'none' && isset($node->{$field['field_name']})) {
    $closing_time = time() + (variable_get('signup_close_early', 1) * 3600);
    if (strtotime($node->{$field['field_name']}[0]['value']) < $closing_time) {
      return TRUE;
    }
  }
  return FALSE;
}

function _signup_date_format_date($node, $include_to_date = FALSE) {
  $field = signup_date_field($node->type);
  if (!$field || $field == 'none') {
    return '';
  }
  if ($field['tz_handling'] == 'date') {
    $tz = $node->{$field['field_name']}[0]['timezone'];
  }
  else {
    $tz = date_default_timezone_name();
  }
  $display_tz = date_get_timezone($field['tz_handling'], $tz);
  $db_tz = date_get_timezone_db($field['tz_handling'], $tz);

  if (isset($node->{$field['field_name']})) {
    $date_value = $node->{$field['field_name']}[0]['value'];
  }
  else {
    $date_value = $node->{$field['database']['columns']['value']['column']};
  }
  $date = date_make_date($date_value, $db_tz, $field['type']);
  if ($db_tz != $display_tz) {
    date_timezone_set($date, timezone_open($display_tz));
  }
  $date_out = date_format_date($date, 'custom', $field['output_format_date']);

  if ($include_to_date) {
    if (isset($node->{$field['field_name']})) {
      $date_value = $node->{$field['field_name']}[0]['value2'];
    }
    else {
      $date_value = $node->{$field['database']['columns']['value2']['column']};
    }
    $date = date_make_date($date_value, $db_tz, $field['type']);
    if ($db_tz != $display_tz) {
      date_timezone_set($date, timezone_open($display_tz));
    }
    $date = date_format_date($date, 'custom', $field['output_format_date']);
    if ($date_value) {
      $date_out .= t(' to ') . date_format_date(date_make_date($date_value), 'custom', $field['output_format_date']);
    }
  }

  return $date_out;
}

