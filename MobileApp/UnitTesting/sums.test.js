const sums = require('../SunBatherApp/app/shared/sums');


test('nissssg', () => {
    expect(sums.sumAdd(1, 2)).toBe(3);
    expect(sums.sumSub(4, 2)).toBe(2);
  });