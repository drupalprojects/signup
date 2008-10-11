/* $Id$ */

/**
 * Conditionally show/hide settings based on the signup form location.
 *
 * If the signup form is being show on the node itself, it's wrapped
 * in a fieldset and there's a setting to control if that fieldset
 * should be collapsed.  Only show this setting if the form is going
 * to be displayed on the node itself.
 *
 * Furthermore, if the signup form is being shown at all, we need to
 * display the setting to control if/how the signup list is generated.
 *
 * Finally, we have to do some complicated logic to hide/show the
 * views-related settings.  If the signup form is being shown at all,
 * and the signup user list is being rendered by an embedded view,
 * show the views settings, otherwise, hide them.
 */
Drupal.signupFormLocationSettingAutoAttach = function () {
  $('div.signup-form-location-radios input.form-radio').click(function () {
    // Simple part: Depending on the form location, hide/show the
    // collapsible fieldset setting.
    if (this.value == 'node') {
      $('div.signup-fieldset_collapsed-setting').show();
    }
    else {
      $('div.signup-fieldset_collapsed-setting').hide();
    }

    // Harder part: All the other settings that are inter-related.
    if (this.value == 'none') {
      // If the signup form is not used, hide everything else.
      $('div.signup-display-signup-user-list-setting').hide();
      $('div.signup-user-list-view-settings').hide();
    }
    else {
      // Signup form is used, so show the user list setting.
      $('div.signup-display-signup-user-list-setting').show();

      // Now, see if the 'embed a view' radio is selected, and use
      // that to show/hide all the view-related settings.
      var embedView = false;
      $('div.signup-display-signup-user-list-setting input.form-radio').each(function() {
        if (this.value == 'embed-view' && this.checked) {
          embedView = true;
        }
      });
      if (embedView) {
        $('div.signup-user-list-view-settings').show();
      }
      else {
        $('div.signup-user-list-view-settings').hide();
      }
    }
  });
};

/**
 * Conditionally show/hide settings based on the signup user list setting.
 *
 * If the signup user list is going to be an embedded view, show the
 * view-related settings, otherwise, hide them.
 */
Drupal.signupUserListSettingAutoAttach = function () {
  $('div.signup-display-signup-user-list-setting input.form-radio').click(function () {
    if (this.value == 'embed-view') {
      $('div.signup-user-list-view-settings').show();
    }
    else {
      $('div.signup-user-list-view-settings').hide();
    }
  });
};

// Global killswitch
if (Drupal.jsEnabled) {
  $(function() {
    Drupal.signupFormLocationSettingAutoAttach();
    Drupal.signupUserListSettingAutoAttach();
  });
}
