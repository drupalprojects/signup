signup allows users to sign up for nodes of any type.  includes options for sending a notification email to a selected email address upon a new user signup (good for notifying event coordinators, etc.), and a confirmation email to users who sign up--these options are per node.  when used on event nodes (with event.module installed), it can also send out reminder emails to all signups X days before the start of the event (per node setting), and auto-close event signups X hours before their start (general setting).  settings exist for resticting signups to selected roles and content types. 

TO INSTALL:
Note: It is assumed that you have Drupal up and running.  Be sure to
check the Drupal web site if you need assistance.  If you run into
problems, you should always read the INSTALL.txt that comes with the
Drupal package and read the online documentation.

:Preparing for Installation:

Note: Backing up your database is a very good idea before you begin!

1. Place the signup package into your Drupal modules/ directory.

2. Load the database definition file (signup.mysql) using the
   tool of your choice (e.g. phpmyadmin). For mysql and command line
   access use:
     mysql -u user -p drupal < signup.mysql

   Replace 'user' with the MySQL username, and 'drupal' with the
   database being used.

3. Enable the signup module by navigating to: administer > modules

	Note: if you want support for auto-closing of events and reminder
	emails you must also install and enable the event package!

   Click the 'Save configuration' button at the bottom to commit your
   changes.

4. For the final configuration of the module, navigate to
   administer > settings > signup

   Here you can configure the options for the module.

5. Enable the node types that you wish to allow signups for under

   administer > settings > content types > configure

   Note--in Drupal 4.6.X, the path is:
   administer > content > configure (it's a tab on the content screen) > 
   content types (a link on the configure screen) > configure (a link
   on the content types screen - whew!).  

   You may wish to create a new content type specifically for event signups.

6. Grant the proper access to user accounts under: administer > access control
   
   allow signups: allows the selected role to sign up for nodes

   NOTE: Enabling 'allow signups' for the anonymous user will only place a 
         login/register link in any signup node that an anonymous user may
         visit.  Signup module does not currently support signups for 
         anonymous users.

   admin signups: allows the selected role to view who has signed up
                  for nodes
   
   viewing the signup report under administer > signup, and configuring
   administer > settings > signup are restricted to user who have the
   'access administration pages' privilege

7. Start signing up!


The following is a wish list for ugrades to this module.  If you'd be interested in
providing financial sponsorship for any of these features, please email
thehunmonkgroup@yahoo.com

//  TODO's
//  1. add support limiting signups to certain roles, per node
//  2. veiwing/manual opening of closed signups
//  3. make datetime display dependent on medium date format display setting in admin/settings
//  4. adding support for custom auto-closing settings per node: time before event start, number of signups
//  5. better header info for emails sent to forwarding email address?
//  6. validation of forwarding email addresses
//  7. validation of user submitted data
//  8. catch remote user logins and get their email addy (currently signup relies on the email addy of a registered user for 
//     confirmation/reminder emails)
//  9. support for admin own signups
//  10. user page support where users can see a list of the events they've signed up for
//  11. ability for admins to cancel signups from the signup tab
//  12. feature to allow event admins to email signup group
//  13. fix reminder message notification so it doesn't display on signups where reminder has already been sent
//  14. make display of reminder email info conditional on whether node is event enabled (form edit page)
//  15. better themed user signup form?