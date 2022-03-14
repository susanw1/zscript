Feature: Core Zcode communication
	The target must support the basic Zcode functionality (R0, R1, R2), and should support the general functionality most Zcode systems can be expected to have (R6, R3), 
	as well as any of the more optional functionality it claims to support, all within the core (system) Zcode space. This does not include any peripherals, 
	although requires that at least one peripheral is supported, as anything else is meaningless.
	
	Background:
		Given the target is running and connected
		And a connection is established to the target
		
	@Standard-operation
  Scenario: the target identifies itself
    When the target is sent an identify command
    Then the target must respond with the expected fields
    
	@Standard-operation
  Scenario: the target gives a summary of its capabilities
    When the target is sent a capability request command
    Then the target must respond with the expected fields
    And the target must support all required functionality
    
	@Standard-operation
  Scenario: the target echos data sent with the echo command
    When the target is sent an echo command with various fields
    Then the target must respond with the expected fields
    
	@Standard-operation
  Scenario: the target echos data sent with the echo command, including multi-byte fields
  	Given the target supports multi-byte fields
    When the target is sent an echo command with multi-byte fields
    Then the target must respond with the expected fields
    
	@Standard-operation
  Scenario: the target echos data sent with the echo command, including an override for status
    When the target is sent an echo command with an override for status
    Then the target must respond with the expected fields
    
	@Standard-operation
  Scenario: the target is given a big string field containing command separators
    When the target is sent a string field containing command separators
    Then the target must respond with the expected fields
    
	@Standard-operation
  Scenario: the target echos data sent with the echo command, to its limits on both fields and big field
  	Given the targets capabilities are known
    When the target is sent an echo command with as much data as possible
    Then the target must respond with the expected fields
    
	@Standard-failure
  Scenario: the target is given more field data to echo than it can process at once
  	Given the targets capabilities are known
  	And the target has limited field size
    When the target is sent an echo command with too much field data
    Then the target must respond with a TOO_BIG status
    
	@Standard-failure
  Scenario: the target is given more big-field data to echo than it can process at once
  	Given the targets capabilities are known
  	And the target has limited big-field size
    When the target is sent an echo command with too much big-field data
    Then the target must respond with a TOO_BIG status
    
	@Standard-failure
  Scenario: the target is given a peripheral command, without being activated
  	Given the targets capabilities are known
  	And the target has at least one peripheral command
    When the target is sent a peripheral command
    Then the target must respond with a NOT_ACTIVATED status
    
	@Standard-operation
  Scenario: the target is given a peripheral command, having been activated
  	Given the targets capabilities are known
  	And the target has at least one harmless peripheral command
    When the target is sent an activate command
    And the target is sent a harmless peripheral command
    Then the target must respond with an OK status
    
	@Standard-operation
  Scenario: the target is given multiple echo commands in a sequence
    When the target is sent multiple echo commands
    Then the target must respond with the expected fields
    
	@Standard-operation
  Scenario: the target is given multiple echo commands in a sequence including error handlers
    When the target is sent multiple echo commands including error handlers
    Then the target must respond with the expected fields
    
	@Standard-failure
  Scenario: the target is given multiple echo commands, one of which gives S10, in a sequence
    When the target is sent multiple echo commands one of which gives S10, in a sequence
    Then the target must respond with the expected fields
    
	@Standard-failure
  Scenario: the target is given multiple echo commands, one of which gives S10, in a sequence with error handler
    When the target is sent multiple echo commands one of which gives S10, in a sequence with error handler
    Then the target must respond with the expected fields
    
	@Standard-failure
  Scenario: the target is given multiple echo commands, one of which gives S5, in a sequence with error handler
    When the target is sent multiple echo commands one of which gives S5, in a sequence with error handler
    Then the target must respond with the expected fields
    