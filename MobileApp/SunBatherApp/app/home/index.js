import { UserViewModel } from './index-view-model'

export function onNavigatingTo(args) {
  const component = args.object
  component.bindingContext = new UserViewModel()
}