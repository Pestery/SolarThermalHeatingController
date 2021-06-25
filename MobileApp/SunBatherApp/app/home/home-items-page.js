import { UserViewModel } from './home-items-view-model'

export function onNavigatingTo(args) {
  const component = args.object
  component.bindingContext = new UserViewModel()
}

// not used
export function onItemTap(args) {
  const view = args.view
  const page = view.page
  const tappedItem = view.bindingContext

  page.frame.navigate({
    moduleName: 'home/home-item-detail/home-item-detail-page',
    context: tappedItem,
    animated: true,
    transition: {
      name: 'slide',
      duration: 200,
      curve: 'ease',
    },
  })
}


