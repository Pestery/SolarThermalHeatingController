// file is used a a proof of concept, if this works then jest is working

const sums = require('./sum');

test('adds 1 + 2 to equal 3', () => {
  expect(sums(1, 2)).toBe(3);
});