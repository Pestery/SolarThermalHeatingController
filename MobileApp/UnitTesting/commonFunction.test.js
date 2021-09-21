const CF = require('../SunBatherApp/app/shared/commonFunctionsMod');

test('Test Convert to Bool', () => {
  expect(CF.convertToBool("autO")).toBe(true);
  expect(CF.convertToBool("On")).toBe(true);
  expect(CF.convertToBool("Manual")).toBe(false);
  expect(CF.convertToBool(undefined)).toBe(false);
  expect(CF.convertToBool("autO")).not.toBe(false);
  expect(CF.convertToBool("On")).not.toBe(false);
  expect(CF.convertToBool("Manual")).not.toBe(true);
  expect(CF.convertToBool(undefined)).not.toBe(true);
});

test('Test Convert bool to On/Off', () => {
  expect(CF.convertOnOff(true)).toBe("On");
  expect(CF.convertOnOff(false)).toBe("Off");
  expect(CF.convertOnOff(true)).not.toBe("Off");
  expect(CF.convertOnOff(false)).not.toBe("On");
});

test('Test Convert bool to auto/manual', () => {
  expect(CF.convertAutoManual(true)).toBe("Auto");
  expect(CF.convertAutoManual(false)).toBe("Manual");
  expect(CF.convertAutoManual(true)).not.toBe("Manual");
  expect(CF.convertAutoManual(false)).not.toBe("Auto");
});

test('Test Add Celcius Symbol', () => {
  expect(CF.addCelcius("25")).toBe("25 C");
  expect(CF.addCelcius("25")).not.toBe("25C");
  expect(CF.addCelcius("25")).not.toBe("24 C");
});

test('Test Add W/M^2 Symbol', () => {
  expect(CF.addUV("25")).toBe("25 W/M^2");
  expect(CF.addUV("25")).not.toBe("25W/M^2");
  expect(CF.addUV("25")).not.toBe("24 W/M^2");
});

test('Test Average Calculator', () => {
  expect(CF.averageRound(10, 2)).toBe("5.0"); // check if average correctly
  expect(CF.averageRound(10, 3)).toBe("3.3"); // check if rounds correctly (would be 3.3333 if no rounding)
  expect(CF.averageRound(10, 0)).toBe(0); // if divide by zero or less, should always return 0
  expect(CF.averageRound(10, -10)).toBe(0);
  expect(CF.averageRound(-10, 10)).toBe("-1.0"); // check if average negative numbers correctly
  expect(CF.averageRound(10, 2)).not.toBe("4.0");
  expect(CF.averageRound(10, 3)).not.toBe("3.33");
  expect(CF.averageRound(10, 0)).not.toBe(10);
  expect(CF.averageRound(10, -10)).not.toBe(-1);
  expect(CF.averageRound(-10, 10)).not.toBe("1.0");
});