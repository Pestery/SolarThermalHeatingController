const sum = require('./sum');
const sums = require('../SunBatherApp/app/shared/sums');


test('adds 1 + 2 to equal 3', () => {
  expect(sum(1, 2)).toBe(3);
});

test('nissg', () => {
  expect(sums.sumAdd(1, 2)).toBe(3);
  expect(sums.sumSub(4, 2)).toBe(2);
});