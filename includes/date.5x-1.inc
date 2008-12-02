<?php
// $Id$


/**
 * @file
 * Code required to support version 5.x-1.* of the CCK date field module.
 */

/**
 *
 * @return Array of SQL clauses for cron reminder email query builder.
 */
function _signup_date_reminder_sql($content_type) {
  require_once drupal_get_path('module', 'date') .'/date.inc';
  global $db_type;
  $day = $db_type == 'pgsql' ? 'days' : 'DAY';

  $field = signup_date_field($content_type);
  $date_type = $field['type'] == 'datestamp' ? 'int' : 'iso';
  $start_time = date_sql('DATE', $field['database']['columns']['value']['column'], $date_type);

  $where = "NOW() + INTERVAL s.reminder_days_before ". $day ." > ". $start_time;
  return array(
    'fields' => array($field['database']['columns']['value']['column']),
    'joins' => array('INNER JOIN {'. $field['database']['table'] .'}  ON {'. $field['database']['table'] .'}.vid = n.vid'),
    'where' => array($where),
  );
}

/**
 *
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function _signup_date_autoclose_sql($content_type) {
  require_once drupal_get_path('module', 'date') .'/date.inc';
  global $db_type;

  $close_early = variable_get('signup_close_early', 1);
  switch ($db_type) {
    case 'mysql':
    case 'mysqli':
      $interval = "INTERVAL '". $close_early ."' HOUR";
      break;

    case 'pgsql':
      $interval = "INTERVAL '". $close_early ." hours'";
      break;

  }

  $field = signup_date_field($content_type);
  $date_type = $field['type'] == 'datestamp' ? 'int' : 'iso';
  $start_time = date_sql('DATE', $field['database']['columns']['value']['column'], $date_type);

  $where = "NOW() + ". $interval ." > ". $start_time;
  return array(
    'fields' => array($field['database']['columns']['value']['column']),
    'joins' => array('INNER JOIN {'. $field['database']['table'] .'} ON {'. $field['database']['table'] .'}.vid = n.vid'),
    'where' => array($where),
  );
}

/**
 * Returns TRUE if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_date_node_completed($node) {
  require_once drupal_get_path('module', 'date') .'/date.inc';
  $field = signup_date_field($node->type);
  if ($field && $field != 'none' && isset($node->{$field['field_name']})) {
    switch ($field['type']) {
      case 'date':
        $start_time = date_iso2unix($node->{$field['field_name']}[0]['value']);
        break;

      case 'datestamp':
        $start_time = $node->{$field['field_name']}[0]['value'];
        break;

    }
    $close_time = $start_time - (variable_get('signup_close_early', 1) * 3600);
    if (time() > $close_time) {
      return TRUE;
    }
  }
  return FALSE;
}

function _signup_date_format_date($node) {
  $field = signup_date_field($node->type);
  if (!$field || $field == 'none') {
    return '';
  }
  if (isset($node->{$field['field_name']})) {
    $date_value = $node->{$field['field_name']}[0]['value'];
  }
  elseif (isset($node->{$field['field_name'] .'_value'})) {
    $date_value = $node->{$field['field_name'] .'_value'};
  }
  else {
    $date_value = $node->{$field['database']['columns']['value']['column']};
  }
  $date_obj = date_make_date($date_value, 'GMT', 'db', $field['type']);
  switch ($field['tz_handling']) {
    case 'site':
      $timezone_name = date_get_site_timezone();
      break;

    case 'date':
      $timezone_name = $field['field_timezone'];
      break;

  }
  if (!empty($timezone_name)) {
    $offset = date_offset(date_gmgetdate($date_obj->db->timestamp), $timezone_name);
  }
  else {
    $offset = 0;
    $timezone_name = NULL;
  }
  return date_format_date($field['output_format_date'], date_fuzzy_stamp($date_obj->db) + $offset, 0, $timezone_name);
}

