<?php
// $Id$

/**
 * Filter nodes based on their signup status (open vs. closed).
 */
class signup_handler_filter_signup_status extends signup_handler_filter_boolean {
  function get_value_options() {
    $this->value_options = array(1 => t('Open'), 0 => t('Closed'));
  }
}

