import { ConnectViewModel } from './connect-view-model'

export function onNavigatingTo(args) {
  const component = args.object
  component.bindingContext = new ConnectViewModel()
}