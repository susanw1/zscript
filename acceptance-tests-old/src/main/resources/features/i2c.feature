Feature: I2C communication
	The target, if it claims to support I2C, should allow write, read and read and compare, along with retries, and bus unlocking.
	Background:
		Given the target claims to support I2C
		And a connection is established to the target
		And an I2C test slave is connected
		
	@Standard-operation
  Scenario: the target performs an echo via the I2C slave
    When the target writes an echo message
    And the target reads 
    Then the response must match the message
    
	@Standard-operation
  Scenario: the target must write and read despite clock stretching
  	Given the slave has been told to clock stretch
    When the target writes an echo message
    And the target reads 
    Then the response must match the message
    
	@Standard-failure
  Scenario: the target performs a write, but no I2C slave is there
    When the target writes to the wrong address
    Then the response must have a NACK on address
    
	@Standard-failure
  Scenario: the target performs a write, but the slave NACKs the data
  	Given the slave is set to NACK the message
    When the target writes a message
    Then the response must have a NACK on data
    
	@Standard-failure
  Scenario: the target performs a write, but the slave NACKs the data, before recovering
  	Given the target supports I2C retries
  	And the slave is set to NACK the first 1 messages
    When the target writes a message
    Then the response must have 1 retries
    
	@Standard-failure
  Scenario: the target performs a write with many retries, but the slave NACKs the data, before recovering
  	Given the target supports I2C retries
  	And the slave is set to NACK the first 10 messages
    When the target writes a message with 10 retries
    Then the response must have 10 retries
    
	@Major-failure
  Scenario: the target performs a read, and the slave locks the bus due to an error
  	Given the slave has been told to lock the bus on read
    When the target reads
    Then the target must recover
    
	@Major-failure
  Scenario: the target performs a read, and the slave locks the bus due to an error, but the target recovers the bus
  	Given the target supports bus recovery
  	And the slave has been told to lock the bus on read
    When the target reads
    Then the target must recover without a reset
    
	@Standard-operation
  Scenario: the target performs a read and compare, which succeeds
  	Given the slave has been primed with an echo message
    When the target performs a read and compare
    Then the response must have status OK
    
	@Standard-operation
  Scenario: the target performs a read and compare, which fails
  	Given the slave has been primed with an echo message
    When the target performs a read and compare for a different message
    Then the response must have status CMD_FAIL
    
	@Standard-operation
  Scenario: the target performs a read and compare, which succeeds due to masking
  	Given the slave has been primed with an echo message
    When the target performs a read and compare for a different message, but with the differences masked
    Then the response must have status OK
    
	@Standard-operation
  Scenario: the target performs a read and compare, which fails despite masking
  	Given the slave has been primed with an echo message
    When the target performs a read and compare for a different message, but with the similarities masked
    Then the response must have status CMD_FAIL
    