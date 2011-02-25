<?php


/**
 * @file
 * Provides support for Views integration.
 *
 * This exposes signup-related data and provides fields, filters,
 * sorts, and arguments.
 *
 * Additionally, default views for standard parts of the signup UI are
 * defined here.  If views is not enabled, these UI elements are
 * implemented in signup/includes/views.none.inc.
 */

/**
 * Private implementation of hook_views_tables().
 */
function _signup_views_tables() {
  $tables['signup_log'] = array(
    'name' => 'signup_log',
    'join' => array(
      'type' => 'inner',
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'nid',
      ),
    ),
    'fields' => array(
      'uid' => array(
        'name' => t('Signup: User: Name'),
        'handler' => 'views_handler_field_uid',
        'sortable' => TRUE,
        'help' => t('Username of an authenticated user who signed up, or %anonymous for anonymous signups.', array('%anonymous' => variable_get('anonymous', t('Anonymous')))),
      ),
      'email' => array(
        'name' => t('Signup: User: Email Address'),
        'handler' => 'views_handler_field_signup_email',
        'sortable' => TRUE,
        'field' => 'uid',
        'addlfields' => array('anon_mail'),
        'help' => t('Email address of a user (authenticated or anonymous) who signed up.') .' <strong>'. t('WARNING: only expose this data in a view that is restricted to users whom you can trust with such sensitive information.') .'</strong>',
      ),
      'signup_time' => array(
        'name' => t('Signup: User: Signup Time'),
        'sortable' => TRUE,
        'handler' => views_handler_field_dates(),
        'option' => 'string',
        'help' => t('Time when user signed up.'),
      ),
      'form_data' => array(
        'name' => t('Signup: User: Additional Signup Info'),
        'handler' => 'views_handler_field_signup_form_data',
        'option' => 'string',
        'help' => t('Enter the additional field name (from signup.theme) in the Option column.'),
      ),
    ),
    'sorts' => array(
      'signup_time' => array(
        'name' => t('Signup: User: Signup Time'),
        'handler' => 'views_handler_sort_date',
        'option' => views_handler_sort_date_options(),
        'help' => t('Sorts by time of sign up.'),
      ),
    ),
    'filters' => array(
      'signup_time' => array(
        'name' => t('Signup: User: Signup Time'),
        'operator' => 'views_handler_operator_gtlt',
        'value' => views_handler_filter_date_value_form(),
        'handler' => 'views_handler_filter_timestamp',
        'option' => 'string',
        'help' => t('Enter dates in the format: CCYY-MM-DD HH:MM:SS. Enter \'now\' to use the current time. You may enter a delta (in seconds) to the option that will be added to the time; this is most useful when combined with now. If you have the jscalendar module from jstools installed, you can use a popup date picker here.'),
      ),
      'signup_user_auth' => array(
        'name' => t('Signup: User: Anonymous/Authenticated'),
        'field' => 'uid',
        'operator' => array('=' => t('is')),
        'value' => array(
          '#type' => 'select',
          '#options' => array(0 => variable_get('anonymous', t('Anonymous')), 1 => t('Authenticated')),
        ),
        'handler' => 'views_handler_filter_signup_uid',
        'help' => t('Filter on if a user who signed up is anonymous, or an authenticated user on the site.'),
      ),
      'signup_user_current' => array(
        'field' => 'uid',
        'name' => t('Signup: User: Current User'),
        'operator' => 'views_handler_operator_eqneq',
        'list' => 'views_handler_filter_usercurrent',
        'list-type' => 'select',
        'help' => t('This allows you to filter by whether or not the node was signed up by the logged in user of the view.'),
      ),
    ),
  );
  $tables['signup'] = array(
    'name' => 'signup',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'nid',
      ),
    ),
    'fields' => array(
      'status' => array(
        'name' => t('Signup: Node: Open/Closed'),
        'sortable' => TRUE,
        'handler' => 'views_handler_field_signup_status',
        'help' => t('Are signups open or closed for this node?'),
      ),
      'close_signup_limit' => array(
        'name' => t('Signup: Node: Signup Limit'),
        'sortable' => TRUE,
        'help' => t('Maximum number of users who can sign up before signups are automatically closed (set to 0 for no limit).'),
      ),
      'forwarding_email' => array(
        'name' => t('Signup: Notification: Email Address'),
        'help' => t('Address where notification emails are sent wenever a user signs up.'),
      ),
      'send_confirmation' => array(
        'name' => t('Signup: Confirmation: Enabled/Disabled'),
        'sortable' => TRUE,
        'help' => t('Should confirmation email be sent to each user who signs up.'),
      ),
      'confirmation_email' => array(
        'name' => t('Signup: Confirmation: Message'),
        'help' => t('The body of the optional confirmation email that can be sent whenever a user signs up.'),
      ),
      'send_reminder' => array(
        'name' => t('Signup: Reminder: Enabled/Disabled'),
        'sortable' => TRUE,
        'help' => t('Should a reminder email be automatically sent to all users who signed up. This will be false if either an administrator disabled the feature for a given event, or if the reminder was already sent.'),
      ),
      'reminder_days_before' => array(
        'name' => t('Signup: Reminder: Days Before Event for Email'),
        'sortable' => TRUE,
        'help' => t('How many days before an event will the reminder email be sent.'),
      ),
      'reminder_email' => array(
        'name' => t('Signup: Reminder: Message'),
        'help' => t('The body of the optional reminder email that can be sent a configurable time before an event begins.'),
      ),
/*
      // Already in the schema, but not implemented in the code, yet:
      'close_in_advance_time' => array(
        'name' => t('Signup: Node: Close in Advance Time'),
        'sortable' => TRUE,
        'handler' => 'views_handler_field_since',
        'help' => t('How many days before an event will the reminder email be sent.'),
      ),
*/
    ),
    'sorts' => array(
      'status' => array(
        'name' => t('Signup: Node: Open/Closed'),
        'help' => t('Sort by if signups are closed or open.'),
      ),
    ),
    'filters' => array(
      'status' => array(
        'name' => t('Signup: Node: Open/Closed'),
        'operator' => array('=' => t('are')),
        'value' => array(
          '#type' => 'select',
          '#options' => array(1 => t('Open'), 0 => t('Closed')),
        ),
        'help' => t('Filter on if signups are open or closed for each node.'),
      ),
      'close_signup_limit' => array(
        'name' => t('Signup: Node: Signup Limit'),
        'operator' => 'views_handler_operator_gtlt',
        'help' => t('Filter by the maximum number of users who can sign up before signups are automatically closed (set to 0 for no limit).'),
      ),
      'signup_disabled' => array(
        'name' => t('Signup: Node: Enabled/Disabled'),
        'field' => 'nid',
        'operator' => array('IS' => t('are')),
        'value' => array(
          '#type' => 'select',
          '#options' => array('NULL' => t('Disabled'), 'NOT NULL' => t('Enabled')),
        ),
        'handler' => 'views_handler_filter_is_null',
        'help' => t('Filter on if signups are enabled or disabled.'),
      ),
    ),
  );
  return $tables;
}

/**
 * Format a uid field as a username.
 */
function views_handler_field_uid($fieldinfo, $fielddata, $value, $data) {
  if (isset($value)) {
    $account = user_load(array('uid' => $value));
    return theme('username', $account);
  }
}

function views_handler_field_signup_email($fieldinfo, $fielddata, $value, $data) {
  if ($value == 0) {
    // Anonymous, the email is included in the $data object already.
    $table = $fieldinfo['table'];
    $field = $table .'_anon_mail';
    return check_plain($data->$field);
  }
  else {
    // Authenticated, have to get the email from the $user object via uid.
    $account = user_load(array('uid' => $value));
    return check_plain($account->mail);
  }
}

/**
 * Retrieves and returns the field from the serialized form_data
 * field, indicated by the admin in the "Option" column.
 */
function views_handler_field_signup_form_data($fieldinfo, $fielddata, $value, $data) {
  $profiledata = unserialize($value);
  return $profiledata[$fielddata['options']];
}

/**
 * Displays a field indicating if signups are open or closed.
 */
function views_handler_field_signup_status($fieldinfo, $fielddata, $value, $data) {
  return $value == 0 ? t('Closed') : t('Open');
}

/**
 * Private implementation of hook_views_arguments().
 */
function _signup_views_arguments() {
  $arguments['signup_uid'] = array(
    'name' => t('Signup: User: UID Signup for Node'),
    'handler' => 'views_handler_arg_signup_uid',
    'help' => t('Filter signups by signed up user.'),
    'option' => array(
      '#type' => 'select',
      '#options' => array(1 => t('Signed up'), 0 => t('Not signed up')),
    ),
  );
  return $arguments;
}

/**
 * An argument handler for signup_uid
 *
 */
function views_handler_arg_signup_uid($op, &$query, $argtype, $arg = '') {
  switch ($op) {
    case 'summary':
      $query->add_table('signup_log', TRUE);
      $query->add_field('uid', 'signup_log');
      $fieldinfo['field'] = "signup_log.uid";
      return $fieldinfo;

    case 'sort':
      $query->add_orderby('users', 'name', $argtype);
      break;

    case 'filter':
      $option = $argtype['options'];
      $uid = intval($arg);
      if ($option) {
        $query->ensure_table('signup_log');
        $query->add_where("signup_log.uid = $uid");
      }
      else {
        $joininfo = array(
          'type' => 'LEFT',
          'left' => array(
            'table' => 'node',
            'field' => 'nid',
          ),
          'right' => array(
            'field' => 'nid',
          ),
          'extra' => array(
            'uid' => $uid,
          ),
        );
        $num = $query->add_table('signup_log', TRUE, 1, $joininfo);
        $tablename = $query->get_table_name('signup_log', $num);
        $query->add_where("$tablename.uid IS NULL");
      }
      break;

    case 'link':
      $name = ($query->name ? $query->name : variable_get('anonymous', t('Anonymous')));
      return l($name, "$arg/". intval($query->uid));

    case 'title':
      if (!$query) {
        return variable_get('anonymous', t('Anonymous'));
      }
      $user = db_fetch_object(db_query("SELECT name FROM {users} WHERE uid = '%d'", $query));
      return $user->name;
  }
}

/**
 * Handler for filters that want to test if a given field IS NULL or
 * IS NOT NULL.
 *
 * @note This function will hopefully be included in views.module itself
 * in the near future, so we only conditionally define it here to prevent
 * conficts when/if it makes it into views core.
 */
if (!function_exists('views_handler_filter_is_null')) {
  function views_handler_filter_is_null($op, $filter, $filterinfo, &$query) {
    $table = $filterinfo['table'];
    $column = $filterinfo['field'];
    $field = "$table.$column";
    $value = $filter['value'];
    // We cannot use %s for our WHERE clause, or you get invalid SQL.
    // So, we must test for valid values directly, to prevent injection.
    if ($value == 'NULL' || $value == 'NOT NULL') {
      $query->ensure_table($table);
      $query->add_where("%s IS $value", $field);
    }
    else {
      // Invalid value passed in.
      // TODO: How should we propagate the error, since we'd just
      // silently ignore this filter right now.
    }
  }
}

function views_handler_filter_signup_uid($op, $filter, $filterinfo, &$query) {
  $table = $filterinfo['table'];
  $column = $filterinfo['field'];
  $field = "$table.$column";
  $value = $filter['value'];
  $query->ensure_table($table);
  if ($value) {
    // Authenticated user
    $query->add_where("%s != 0", $field);
  }
  else {
    // Anonymous user
    $query->add_where("%s = 0", $field);
  }
}

/**
 * Implementation of hook_views_url_tokens().
 */
function signup_views_url_tokens() {
  return array(
    '$signup' => 'signup_url_node',
  );
}

/**
 * Views URL token handler for $signup that works for signup-enabled nodes.
 *
 * @param $token
 *   Ignored.
 * @param $argument
 *   Ignored.
 * @param $arg
 *   The corresponding element of the URL that the token is holding.
 *
 * @return
 *   TRUE if the path element this token represents is the node ID (nid) of a
 *   signup-enabled node, otherwise FALSE.
 */
function signup_url_node($token, $argument, $arg) {
  if (!is_numeric($arg)) {
    return FALSE;
  }
  $node = node_load($arg);
  if (!$node) {
    return FALSE;
  }
  return $node->signup ? TRUE : FALSE;
}

