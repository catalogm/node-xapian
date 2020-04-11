
const xapian = require('xapian');

test('xapian module loads', () => {
  expect(xapian).toBeDefined();
});

test('xapian module contains objects', () => {
  const expected = [
    'WritableDatabase',
    'Database',
    'Document',
    'Enquire',
    'MSet',
    'MSetIterator',
    'TermIterator',
    'TermGenerator',
    'Stem',
    'Query',
    'QueryParser',
  ];
  expect(Object.keys(xapian)).toEqual(expect.arrayContaining(expected));
});
