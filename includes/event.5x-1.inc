<?php


/**
 * @file
 * Code required to support version 5.x-1.* of the event module.
 */

/**
 *
 * @return Array of SQL clauses for cron reminder email query builder.
 */
function _signup_event_reminder_sql($content_type) {
  // We must manually include this here as the event module doesn't
  // include timezone support on all page requests.
  require_once drupal_get_path('module', 'event') .'/event_timezones.inc';
  return array(
    'fields' => array('e.event_start', 'e.timezone'),
    'joins' => array('INNER JOIN {event} e ON e.nid = n.nid'),
    'where' => array(
      '('. time() .' > (e.event_start - ((s.reminder_days_before) * 86400)))',
      '('. time() .' <= e.event_start)',
    ),
  );
}

/**
 *
 * @return Array of SQL clauses for cron auto-close query builder.
 */
function _signup_event_autoclose_sql($content_type) {
  // We must manually include this here as the event module doesn't
  // include timezone support on all page requests.
  require_once drupal_get_path('module', 'event') .'/event_timezones.inc';
  return array(
    'fields' => array('e.event_start', 'e.timezone'),
    'joins' => array('INNER JOIN {event} e ON e.nid = s.nid'),
    'where' => array('('. time() .' > (e.event_start - '. (variable_get('signup_close_early', 1) * 3600) .'))'),
  );
}

/**
 *
 * @return Array of SQL clauses for admin overview page query builder.
 */
function _signup_event_admin_sql($content_type = NULL) {
  // Since all event node types store their event data in the same table, we
  // need to make sure we only JOIN on this table once.  It doesn't matter
  // which node type we use to add this JOIN.
  static $did_event_join = FALSE;
  if ($did_event_join) {
    return array();
  }
  $did_event_join = TRUE;
  $fields = array('e.event_start', 'e.timezone');
  return array(
    'fields' => $fields,
    // All of the fields should go directly into GROUP BY.
    'group_by' => $fields,
    'joins' => array('LEFT JOIN {event} e ON e.nid = n.nid'),
  );
}

/**
 * Returns TRUE if the given node is event-enabled, and the start time
 * has already passed the "Close x hours before" setting.
 */
function _signup_event_node_completed($node) {
  if (isset($node->event_start)) {
    $closing_time = $node->event_start - (variable_get('signup_close_early', 1) * 3600);
    if (time() > $closing_time) {
      return TRUE;
    }
  }
  return FALSE;
}

function _signup_event_format_date($node) {
  require_once drupal_get_path('module', 'event') .'/event_timezones.inc';
  $offset = intval(variable_get('date_default_timezone', 0));
  $format = _signup_get_date_format();
  return $node->event_start ? _event_date($format, $node->event_start, $offset) : t('[Untimed]');
}

/**
 * Helper function to get the date format string to use based on settings.
 *
 * Sadly, there's nothing in core to provide this info based on the
 * core format settings (small, medium, large), so this code is lifted
 * from format_date() from includes/common.inc in core.  This is
 * necessary for the event module 5.x-1.* backend, since event uses
 * its own proprietary _event_date() function instead of
 * format_date().
 *
 * @return
 *   The date format string to used based on the current value of the
 *   'signup_date_format' setting and the core format strings.
 */
function _signup_get_date_format() {
  $format = '';
  switch (variable_get('signup_date_format', 'small')) {
    case 'small':
      $format = variable_get('date_format_short', 'm/d/Y - H:i');
      break;

    case 'large':
      $format = variable_get('date_format_long', 'l, F j, Y - H:i');
      break;

    case 'custom':
      // No change to format
      break;

    case 'medium':
    default:
      $format = variable_get('date_format_medium', 'D, m/d/Y - H:i');
  }
  return $format;
}

/**
 * Determine if the given node has a start time managed by the event.module.
 *
 * @param $node
 *   Fully loaded node object to test.
 *
 * @return
 *   'event' if this is an event node, otherwise 'none'.
 *
 * @see _signup_get_node_scheduler()
 */
function _signup_event_get_node_scheduler($node) {
  return isset($node->event_start) ? 'event' : 'none';
}

