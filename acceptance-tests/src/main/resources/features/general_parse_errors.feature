Feature: General parse errors
	The target must fail correctly given a variety of wrong commands, without failing dramatically
	
	Background:
		Given the target is running and connected
		And a connection is established to the target
		
	@Standard-failure
  Scenario: the target is given the same field twice
    When the target is sent an echo command with the same field twice
    Then the target must respond with a PARSE_ERROR status
    
	@Standard-failure
  Scenario: the target is given no R field
    When the target is sent an command with no R field
    Then the target must respond with an UNKNOWN_CMD status
    
	@Standard-failure
  Scenario: the target is given an unrecognised R field
    When the target is sent an command with an unrecognised R field
    Then the target must respond with an UNKNOWN_CMD status
    
	@Standard-failure
  Scenario: the target is given two big fields
    When the target is sent an command with two big fields
    Then the target must respond with a PARSE_ERROR status
    
	@Standard-failure
  Scenario: the target is given an unterminated big string
    When the target is sent an command with an unterminated big string
    Then the target must respond with a PARSE_ERROR status
    
	@Standard-failure
  Scenario: the target is given an odd digit big field
    When the target is sent an command with an odd digit big field
    Then the target must respond with a PARSE_ERROR status
    
	@Standard-operation
  Scenario: the target is given debug info
    When the target is sent debug info
    Then the target must not respond
    
	@Standard-operation
  Scenario: the target is given a broadcast command
    When the target is sent a broadcast command
    Then the target must respond with a broadcast response
    
	@Standard-failure
  Scenario: the target is given a broadcast command mid sequence
    When the target is sent a broadcast command mid sequence
    Then the target must respond with a PARSE_ERROR status after expected fields
    
	@Standard-failure
  Scenario: the target is given debug info mid sequence
    When the target is sent debug info mid sequence
    Then the target must respond with a PARSE_ERROR status after expected fields
    
	@Standard-failure
  Scenario: the target is set to parallel mid sequence
    When the target is set to parallel mid sequence
    Then the target must respond with a PARSE_ERROR status after expected fields
    
	@Standard-operation
  Scenario: the target is given an escaped quote in a string
    When the target is sent an command with an escaped quote in a string
    Then the target must respond with a OK status
    
	@Standard-operation
  Scenario: the target is given a non hex character in the wrong place
    When the target is sent an command with a non hex character in the wrong place
    Then the target must respond with an PARSE_ERROR status
    