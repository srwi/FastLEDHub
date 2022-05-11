module.exports = {
  env: {
    browser: true,
    es6: true,
  },
  parserOptions: {
    ecmaVersion: 11,
  },
  rules: {
    semi: ["error", "always"],
    quotes: ["error", "double"],
    eqeqeq: "error",
    "no-undef": "off",
  },
  extends: "eslint:recommended",
};
