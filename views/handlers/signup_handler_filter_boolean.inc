<?php
// $Id$


/**
 * @file
 * Code to fix the UI of signup's boolean views filters.
 */

/**
 * Child class of views_handler_filter_boolean_operator with a better UI.
 *
 * This can be removed once http://drupal.org/node/336531 is incorporated into
 * an official release of views.
 */
class signup_handler_filter_boolean extends views_handler_filter_boolean_operator {
  function construct() {
    $this->value_options = NULL;
    parent::construct();
  }

  function get_value_options() {
    $this->value_options = array(1 => t('Yes'), 0 => t('No'));
  }

  function value_form(&$form, &$form_state) {
    if (empty($this->value_options)) {
      // Initialize the array of possible values for this filter.
      $this->get_value_options();
    }
    if (!empty($form_state['exposed'])) {
      // Exposed filter: use a select box to save space.
      $filter_form_type = 'select';
    }
    else {
      // Configuring a filter: use radios for clarity.
      $filter_form_type = 'radios';
    }
    $form['value'] = array(
      '#type' => $filter_form_type,
      '#title' => $this->value_value,
      '#options' => $this->value_options,
      '#default_value' => $this->value,
    );
    if (!empty($this->options['exposed'])) {
      $identifier = $this->options['expose']['identifier'];
      if (!isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }
    }
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }
    if (empty($this->value_options)) {
      $this->get_value_options();
    }
    // Now that we have the valid options for this filter, just return the
    // human-readable label based on the current value.  The value_options
    // array is keyed with either 0 or 1, so if the current value is not
    // empty, use the label for 1, and if it's empty, use the label for 0.
    return $this->value_options[!empty($this->value)];
  }
}

