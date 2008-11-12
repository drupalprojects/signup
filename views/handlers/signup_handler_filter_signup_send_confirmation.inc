<?php
// $Id$

/**
 * Filter nodes based on if confirmation emails should be sent.
 */
class signup_handler_filter_signup_send_confirmation.inc extends views_handler_filter_boolean_operator {
  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    return (empty($this->value) ? t('Disabled') : t('Enabled'));
  }
}

