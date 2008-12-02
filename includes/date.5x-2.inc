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
  // Get the date field information for this content type.
  $field = signup_date_field($content_type);
  $start_field = $field['database']['columns']['value']['column'];

  // Figure out what TZ we want to do the date comparisons in.
  $compare_tz = $field['tz_handling'] == 'none' ? date_default_timezone_name() : 'UTC';
  // Get a DateAPI SQL handler class for this field.
  // TODO
  $handler = _signup_date_sql_handler($field, $compare_tz);

  // Find the current time in the appropriate TZ for this field.
  $now_date = date_now($compare_tz);
  // Need to enclose this in ' marks to use directly in the SQL.
  $now = "'". date_format($now_date, DATE_FORMAT_DATETIME) ."'";

  // Extract the correct SQL to represent the start time.
  $start_time = $handler->sql_field($start_field);
  // Work-around evil code in date_api_sql that tries to workaround a views
  // bug but breaks other callers. :(
  $start_time = strtr($start_time, array('***SQLD***' => '%%d', '***SQLS***' => '%%s'));

  // Create SQL to represent the time we should start sending reminders, based
  // on the SQL for the start time and the reminder_days_before field.
  $reminder_start = _signup_date_sql_math($start_time, 'SUB', 's.reminder_days_before', 'DAY');
  $reminder_stop = _signup_date_sql_math($start_time, 'ADD', 1, 'HOUR');

  // The WHERE clauses are now trivial: We want to make sure a) the current
  // time is after the time we should start sending reminders, but before the
  // actual start time itself.
  $where = array(
    "$now >= $reminder_start",
    "$now <= $reminder_stop",
  );

  // See what fields to SELECT.
  $fields[] = $start_field;
  if (isset($field['database']['columns']['timezone']['column'])) {
    $fields[] = $field['database']['columns']['timezone']['column'];
  }
  $table = '{'. $field['database']['table'] .'}';
  return array(
    'fields' => $fields,
    'joins' => array("INNER JOIN $table ON $table.vid = n.vid"),
    'where' => $where,
  );
}

/**
 *
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function _signup_date_autoclose_sql($content_type) {
  // Get the date field information for this content type.
  $field = signup_date_field($content_type);
  $start_field = $field['database']['columns']['value']['column'];

  // Figure out what TZ we want to do the date comparisons in.
  $compare_tz = $field['tz_handling'] == 'none' ? date_default_timezone_name() : 'UTC';
  // Get a DateAPI SQL handler class for this field.
  $handler = _signup_date_sql_handler($field, $compare_tz);

  // Compute a string representing the moment when signups should start
  // auto-closing.  If the field has no TZ handling, we just want to grab the
  // current local time.  If the field has any TZ handling, the date will be
  // stored in the DB in UTC time, so start from current UTC time.  Once we
  // have the right current time, we need to add our close-in-advance offset.
  $close_early_hours = variable_get('signup_close_early', 1);
  $close_date = date_now($compare_tz);
  date_modify($close_date, "+$close_early_hours hours");
  $close_date_str = date_format($close_date, DATE_FORMAT_DATETIME);

  // Use the DateAPI SQL handler to construct an appropriate WHERE clause.
  // Make sure that the start time is <= NOW plus the auto-close window.
  $where = $handler->sql_where_date('DATE', $start_field, '<=', $close_date_str);
  // Work-around evil code in date_api_sql that tries to workaround a views
  // bug but breaks other callers. :(
  $where = strtr($where, array('***SQLD***' => '%%d', '***SQLS***' => '%%s'));

  // See what fields to SELECT.
  $fields[] = $start_field;
  if (isset($field['database']['columns']['timezone']['column'])) {
    $fields[] = $field['database']['columns']['timezone']['column'];
  }
  $table = '{'. $field['database']['table'] .'}';
  return array(
    'fields' => $fields,
    'joins' => array("INNER JOIN $table ON $table.vid = n.vid"),
    'where' => $where,
  );
}

/**
 * Returns TRUE if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_date_node_completed($node) {
  $field = signup_date_field($node->type);
  if ($field && $field != 'none' && isset($node->{$field['field_name']})) {
    // Grab whatever date value we actually have, regardless of format.
    $date_value = $node->{$field['field_name']}[0]['value'];
    // Figure out the timezone handling for this date.
    if ($field['tz_handling'] == 'date') {
      $tz = $node->{$field['field_name']}[0]['timezone'];
    }
    else {
      $tz = date_default_timezone_name();
    }
    $db_tz = date_get_timezone_db($field['tz_handling'], $tz);
    // Create a date object
    $date = date_make_date($date_value, $db_tz, $field['type']);
    // Make sure the date object is going to print UTC values.
    date_timezone_set($date, timezone_open('UTC'));
    // Find out how early signups should be automatically closed.
    $close_early_hours = variable_get('signup_close_early', 1);
    date_modify($date, "-$close_early_hours hours");
    $close_time = date_format($date, 'U');
    // Find the current UTC time.
    $now = date_now('UTC');
    if (date_format($now, 'U') >= $close_time) {
      // It's now later than when this node would automatically close signups.
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
    if (isset($node->{$field['field_name']})) {
      $tz = $node->{$field['field_name']}[0]['timezone'];
    }
    else {
      $tz = $node->{$field['database']['columns']['timezone']['column']};
    }
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

/**
 * Generate a DateAPI SQL handler for the given CCK date field.
 *
 * This can be removed once revision 1.39.2.48 of date/date.module is widely
 * available in an official release.
 */
function _signup_date_sql_handler($field, $compare_tz = NULL) {
  // Workaround for a bug in DateAPI upto 6.x-2.0-rc5.
  static $mysql_db_offset_set = FALSE;
  require_once drupal_get_path('module', 'date_api') .'/date_api_sql.inc';

  // Create a DateAPI SQL handler class for this field type.
  $handler = new date_sql_handler();
  $handler->construct($field['type']);

  // If this date field stores a timezone in the DB, tell the handler about it.
  if ($field['tz_handling'] == 'date') {
    // The field has a date column
    $handler->db_timezone_field = $field['database']['columns']['timezone']['column'];
  }
  else {
    $handler->db_timezone = date_get_timezone_db($field['tz_handling']);
  }

  if (empty($compare_tz)) {
    $compare_tz = date_get_timezone($field['tz_handling']);
  }
  $handler->local_timezone = $compare_tz;

  // Now that the handler is properly initialized, tell the DB what TZ to use.
  $handler->set_db_timezone();

  // Up to and including DateAPI 5.x-2.4, the previous call only worked
  // for mysqli and pgsql. It was a no-op on mysql itself, so in that case, we
  // have to do the work ourselves here or datestamp fields don't work.
  if ($GLOBALS['db_type'] == 'mysql' && !$mysql_db_offset_set && version_compare(db_version(), '4.1.3', '>=')) {
    db_query("SET @@session.time_zone = '+00:00'");
    $mysql_db_offset_set = TRUE;
  }

  return $handler;
}

/**
 * Helper function to handle date math across DB types.
 *
 * This can be removed once date_api_sql.inc revision 1.4.2.35 is widely
 * available in an official release.
 *
 * @param $field
 *   The field to be adjusted.
 * @param $direction
 *   Either ADD or SUB.
 * @param $count
 *   The number of values to adjust.
 * @param $granularity
 *   The granularity of the adjustment, should be singular,
 *   like SECOND, MINUTE, DAY, HOUR.
 */
function _signup_date_sql_math($field, $direction, $count, $granularity) {
  $granularity = strtoupper($granularity);
  switch ($GLOBALS['db_type']) {
    case 'mysql':
    case 'mysqli':
      if ($direction == 'ADD') {
        return "DATE_ADD($field, INTERVAL $count $granularity)";
      }
      else {
        return "DATE_SUB($field, INTERVAL $count $granularity)";
      }
      
    case 'pgsql':
      $granularity .= 'S';
      if ($direction == 'ADD') {
        return "($field + INTERVAL '$count $granularity')";
      }
      else {
        return "($field - INTERVAL '$count $granularity')";
      }
  }
  return $field;
}

