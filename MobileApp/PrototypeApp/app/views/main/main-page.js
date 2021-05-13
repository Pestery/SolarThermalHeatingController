import { UserViewModel } from "../shared/view-models/user-view-model";

// UserViewModel is binded to the pages context, this format may need to be changed when moving forward - jack
export function onNavigatingTo(args) {
  const page = args.object
  page.bindingContext = UserViewModel()
}
