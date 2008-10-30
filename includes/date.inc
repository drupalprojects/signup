<?php
// $Id$

/**
 * @file
 * Shared code required for any site using CCK date fields, regardless of the
 * version of date in use.
 */

/**
 * @return Array of SQL clauses for admin overview page query builder.
 */
function signup_admin_sql($content_type_field = NULL) {
  return array(
    'fields' => array('n.type', $content_type_field['database']['columns']['value']['column']),
    'joins' => array('LEFT JOIN {'. $content_type_field['database']['table'] .'} ON {'. $content_type_field['database']['table'] .'}.vid = n.vid'),
  );
}

function signup_admin_form_header() {
  // TODO: Not sure if we can sort date fields, since they might be coming
  // from different tables so for now 'field' is NULL to disable sorting on
  // the date field
  return array('data' => t('Start'), 'field' => NULL);
}

function signup_admin_form_extra($signup_event) {
  return array(
    '#type' => 'markup',
    '#value' => signup_format_date($signup_event)
  );
}

function signup_field_names($content_type = NULL) {
  $fields = array();
  $content_type_info = _content_type_info();
  if ($content_type_info['content types'][$content_type]) {
    foreach ($content_type_info['content types'][$content_type]['fields'] as $field) {
      if (in_array($field['type'], array('date', 'datestamp'))) {
        $fields[$field['field_name']] = $field['widget']['label'];
      }
    }
  }
  return $fields;
}

function signup_extra_tokens() {
  return array('%time');
}

function signup_date_field($content_type) {
  $field_name = variable_get('signup_date_field_'. $content_type, 0);
  if ($field_name === 0 || $field_name == '0') {
    // PHP is completely evil and 'none' == 0 is TRUE, hence the extra checks.
    return FALSE;
  }
  if ($field_name == 'none') {
    return 'none';
  }
  $field = content_fields($field_name, $content_type);
  if (empty($field)) {
    return array();
  }
  $field['database'] = content_database_info($field);
  return $field;
}

function theme_signup_event_dates($node) {
  return signup_format_date($node, TRUE);
}

/**
 * Returns a list of all cck fields that have been set for use in signups
 */
function signup_content_type_fields() {
  $fields = array();
  foreach (signup_content_types() as $content_type) {
    $field = signup_date_field($content_type);
    if (!empty($field) && $field != 'none') {
      $fields[] = $field;
    }
  }
  return $fields;
}

/**
 * Does this site have any date-enabled content?
 *
 * @return Bool: Always return TRUE in this backend
 */
function signup_site_has_dates() {
  return TRUE;
}

/**
 * @return Bool: Is this node type date-enabled?
 */
function signup_node_type_has_date($type) {
  $signup_date_field = variable_get('signup_date_field_'. $type, 0);
  return (!empty($signup_date_field) && $signup_date_field != 'none');
}

/**
 * @return Bool: Is this specific node date-enabled?
 */
function signup_node_has_date($node) {
  if (signup_node_type_has_date($node->type)) {
    $field = signup_date_field($node->type);
    return isset($node->{$field['field_name']});
  }
  return FALSE;
}

/**
 * Alter the form for configuring CCK date fields on node types.
 *
 * Hooks into CCK Date fields to provide an option to use the current
 * field as the Signup date field (for autoclose and reminder emails).
 *
 * @see signup_form_alter()
 */
function _signup_date_field_form_alter($form_id, &$form) {
  $type = $form['type_name']['#value'];
  if (in_array($form['field_type']['#value'], array('date', 'datestamp')) && variable_get('signup_node_default_state_'. $type, 'disabled') != 'disabled') {
    $form['signup'] = array(
      '#type' => 'fieldset',
      '#title' => t('Signup settings'),
      '#collapsible' => TRUE,
      '#weight' => 1,
    );
    $form['signup']['signup_date_field'] = _signup_date_field_element($type);
    $form['#submit']['_signup_date_field_form_submit'] = array();
    // Make sure the submit button comes after the signup settings fieldset.
    $form['submit']['#weight'] = 50;
  }
}

/**
 * Custom submit handler for the CCK date field editing form.
 *
 * @see _signup_date_field_form_alter()
 */
function _signup_date_field_form_submit($form_id, $form_values) {
  $type = $form_values['type_name'];
  if (empty($form_values['signup_date_field'])) {
    variable_del('signup_date_field_'. $type);
  }
  else {
    variable_set('signup_date_field_'. $type, $form_values['signup_date_field']);
  }
}

/**
 * Alter the node type form to add a setting to select the signup date field.
 *
 * @see signup_alter_node_type_form()
 */
function _signup_date_alter_node_type_form($form_id, &$form) {
  drupal_add_js(drupal_get_path('module', 'signup') .'/js/admin.content_types.js');
  drupal_add_css(drupal_get_path('module', 'signup') .'/signup.css');

  $type = $form['#node_type']->type;
  $default_signup_state = variable_get('signup_node_default_state_'. $type, 'disabled');

  // Add a div to the 'Signup options' radios for signup.date.js.
  $form['workflow']['signup_node_default_state']['#prefix'] = '<div class="signup-node-default-state-radios">';
  $form['workflow']['signup_node_default_state']['#suffix'] = '</div>';

  // Figure out if we should hide the date field selector by default.
  $class = 'signup-date-field-setting';
  if ($default_signup_state == 'disabled') {
    $class .= ' js-hide';
  }

  $form['workflow']['signup_date_field'] = _signup_date_field_element($type);
  $form['workflow']['signup_date_field']['#prefix'] = '<div class="'. $class .'">';
  $form['workflow']['signup_date_field']['#suffix'] = '</div>';
}

/**
 * Create the FAPI form element for the signup date field.
 *
 * @param $type
 *   The node type to generate the form element for.
 *
 * @return
 *   FAPI form array for the signup date field element.
 *
 * @see _signup_date_field_form_alter()
 * @see _signup_date_alter_node_type_form()
 */
function _signup_date_field_element($type) {
  return array(
    '#type' => 'select',
    '#title' => t('Date field to use with signup'),
    '#options' => _signup_get_date_field_options($type),
    '#default_value' => variable_get('signup_date_field_'. $type, 0),
    '#description' => t('Select the date field of this content type to use for signup time-based functionality, such as automatically closing signups when the start time has passed and sending reminder emails. Select "%none" to not use a date field for signup functionality at all.', array('%none' => t('None'))),
  );
}

/**
 * Check the signup and date configuration on node types depending on the URL.
 *
 * This function is invoked from signup_help() so that we can check the
 * configuration of any signup-enabled node types to ensure that the CCK date
 * field and signup settings make sense.
 *
 * @param $type
 *   The 4th element in the URL which specifies which node type is currently
 *   being configured.  If this is empty, it means we're at the node type
 *   overview listing and we should test all node types.
 *
 * @see signup_help()
 * @see signup_date_field_check_config()
 */
function signup_date_check_node_types($type) {
  $names = node_get_types('names');
  if (!empty($type)) {
    signup_date_field_check_config($type, $names[$type]);
  }
  else {
    foreach ($names as $type => $name) {
      signup_date_field_check_config($type, $name);
    }
  }
}

/**
 * Check that the date and signup configuration for a node type makes sense.
 *
 * This validates that if a node type is signup enabled, that it either has a
 * signup date field selected (for autoclose and reminder emails), or that the
 * signup date field has been explicitly set to 'None'.  It warns the site
 * administrator if they have signup-enabled a node type and not defined any
 * date fields at all, or if they have date fields but haven't selected the
 * one to use for signup functionality.
 *
 * @param $type
 *   The node type to check signup and CCK date field configuration on.
 * @param $name
 *   Human readable name of the node type to check.
 *
 * @return
 *   Nothing -- configuration errors are reported via drupal_set_message().
 *
 * @see signup_help()
 */
function signup_date_field_check_config($type, $name) {
  $signup_default_state = variable_get('signup_node_default_state_'. $type, 'disabled');
  if ($signup_default_state != 'disabled') {
    // Signups aren't disabled on this node type, see if there's a date field.
    $signup_date_field = signup_date_field($type);
    if ($signup_date_field != 'none') {
      // Administrator hasn't specifically turned off date support...
      if (signup_field_names($type)) { 
        // Node type has some date fields...
        if ($signup_date_field == 0) {
          drupal_set_message(t('You have enabled the %node_type content type for signups, and have added one or more date fields, but have not selected a date field for use with signup.', array('%node_type' => $name)), 'warning');
        }
      }
      else {
        // No date fields at all.
        drupal_set_message(t('You have enabled the %node_type content type for signups but have not added a date field.', array('%node_type' => $name)), 'warning');
      }
    }
  }
}

/**
 * Helper function for the date field select to build its options.
 *
 * @param $type
 *   Content type whose date fields should be listed.
 *
 * @return
 *   Associative array with all date fields of the given content type plus
 *   'None' and an optional 'Not specified' if the user never selected a
 *   value.
 */
function _signup_get_date_field_options($type) {
  $options = array();
  // Add "Not specified" if the user never selected a field.
  if (variable_get('signup_date_field_'. $type, 0) == 0) {
    $options = array(0 => t('<Not specified>'));
  }
  // Add any date fields from this node type.
  $options += signup_field_names($type);
  // Always add 'None' as the final choice.
  $options += array('none' => t('<None>'));
  return $options;
}