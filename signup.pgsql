
-- 
-- Table structure for table `signup`
--

CREATE TABLE signup (
  nid SERIAL,
  forwarding_email text NOT NULL default '',
  send_confirmation integer NOT NULL default '0',
  confirmation_email text NOT NULL default '',
  send_reminder integer NOT NULL default '0',
  reminder_days_before integer NOT NULL default '0',
  reminder_email text NOT NULL default '',
  close_in_advance_time integer NOT NULL default '0',
  close_signup_limit integer NOT NULL default '0',
  completed integer NOT NULL default '0',
  PRIMARY KEY (nid)
);

-- 
-- Dumping data for table `signup`
--

INSERT INTO signup (nid, forwarding_email,
  send_confirmation, confirmation_email, send_reminder,
  reminder_days_before, reminder_email, close_in_advance_time,
  close_signup_limit, completed) VALUES (0, '', 1,
  'Enter your default confirmation email message here', 1, 0,
  'Enter your default reminder email message here', 0, 0, 0
);

-- 
-- Table structure for table `signup_log`
--

CREATE TABLE signup_log (
  uid integer NOT NULL default '0',
  nid integer NOT NULL default '0',
  signup_time integer NOT NULL default '0',
  form_data text NOT NULL default '',
  UNIQUE (uid,nid)
);