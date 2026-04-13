import { getRequestConfig } from "next-intl/server";
export default getRequestConfig(async () => {
  return {
    locale: "en",
    timeZone: "Asia/Jakarta",
    messages: (await import("../../messages/en.json")).default,
  };
});
